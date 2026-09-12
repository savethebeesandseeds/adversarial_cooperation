#!/usr/bin/env python3
"""Verify moved files against a pre-move inventory without changing any files.

The inventory and move manifest are evidence supplied by the caller, not
independently authenticated records. Only moved files are opened for hashing.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path, PureWindowsPath
import re
import stat
import sys


KINDS = {"tracked-source", "local-archive", "local-evidence", "local-private"}


def path_key(value):
    return os.path.normcase(value).replace("\\", "/")


def relative_path(value):
    """Accept portable root-relative paths; never normalize away traversal."""
    if not isinstance(value, str) or not value:
        raise ValueError("manifest and inventory paths must be nonempty strings")
    value = value.replace("\\", "/")
    if (PureWindowsPath(value).anchor or value.startswith("/")
            or any(part in {"", ".", ".."} for part in value.split("/"))
            or "\x00" in value or ":" in value):
        raise ValueError(f"invalid root-relative path: {value!r}")
    return value


def reject_links(path):
    """Refuse symbolic links and Windows reparse points in existing ancestors."""
    for component in reversed((path, *path.parents)):
        try:
            info = component.lstat()
        except FileNotFoundError:
            break
        if (stat.S_ISLNK(info.st_mode)
                or getattr(info, "st_file_attributes", 0)
                & getattr(stat, "FILE_ATTRIBUTE_REPARSE_POINT", 0x400)):
            raise ValueError(f"link or reparse point refused: {component}")


def contained_path(root, name):
    path = root.joinpath(*name.split("/"))
    reject_links(path)
    if not path.resolve().is_relative_to(root):
        raise ValueError(f"path leaves repository root: {name!r}")
    return path


def read_json(path):
    path = path.absolute()
    reject_links(path)
    with path.open(encoding="utf-8-sig") as stream:
        return json.load(stream)


def load_moves(value):
    if not isinstance(value, list) or not value:
        raise ValueError("moves must be a nonempty JSON array")
    moves = []
    sources = set()
    for item in value:
        if (not isinstance(item, dict) or not isinstance(item.get("kind"), str)
                or item["kind"] not in KINDS):
            raise ValueError("each move needs a recognized kind")
        source = relative_path(item.get("source"))
        destination = relative_path(item.get("destination"))
        source_key = path_key(source)
        if source_key in sources:
            raise ValueError(f"duplicate move source: {source!r}")
        if source_key == path_key(destination):
            raise ValueError(f"source equals destination: {source!r}")
        sources.add(source_key)
        moves.append((source, destination))
    return sorted(moves, key=lambda move: len(move[0].split("/")), reverse=True)


def verify(root, inventory, moves):
    if not isinstance(inventory, dict) or not isinstance(inventory.get("files"), list):
        raise ValueError("inventory must contain a files array")
    failures = []
    affected = []
    final_paths = {}
    inventory_paths = set()
    matches = {source: 0 for source, _ in moves}

    for item in inventory["files"]:
        if not isinstance(item, dict):
            raise ValueError("each inventory file must be an object")
        source = relative_path(item.get("path"))
        key = path_key(source)
        if key in inventory_paths:
            raise ValueError(f"duplicate inventory path: {source!r}")
        inventory_paths.add(key)
        destination = source
        for prefix, target in moves:
            prefix_key = path_key(prefix)
            if key == prefix_key or key.startswith(prefix_key + "/"):
                destination = target + source[len(prefix):]
                matches[prefix] += 1
                if (not isinstance(item.get("sha256"), str)
                        or not re.fullmatch(r"[0-9a-fA-F]{64}", item["sha256"])
                        or type(item.get("bytes")) is not int or item["bytes"] < 0
                        or type(item.get("tracked")) is not bool):
                    raise ValueError(f"invalid inventory metadata: {source!r}")
                affected.append((source, destination, item))
                break
        key = path_key(destination)
        if key in final_paths:
            failures.append(f"destination collision: {final_paths[key]!r} and {source!r}")
        else:
            final_paths[key] = source

    # A file cannot also serve as another final file's parent directory.
    for key, source in final_paths.items():
        parent = key.rpartition("/")[0]
        while parent:
            if parent in final_paths:
                failures.append(f"file/directory collision: {source!r} and {final_paths[parent]!r}")
            parent = parent.rpartition("/")[0]

    absent = 0
    for source, destination in moves:
        try:
            old = contained_path(root, source)
            contained_path(root, destination)
            if old.exists():
                failures.append(f"source still exists: {source!r}")
            else:
                absent += 1
            if not matches[source]:
                failures.append(f"no inventory files matched move: {source!r}")
        except (OSError, ValueError) as error:
            failures.append(str(error))

    verified = 0
    for source, destination, item in affected:
        try:
            path = contained_path(root, destination)
            if not path.is_file():
                raise ValueError(f"destination file missing: {destination!r}")
            if path.stat().st_size != item["bytes"]:
                raise ValueError(f"byte length changed: {source!r} -> {destination!r}")
            digest = hashlib.sha256()
            with path.open("rb") as stream:
                for block in iter(lambda: stream.read(1024 * 1024), b""):
                    digest.update(block)
            if digest.hexdigest() != item["sha256"].lower():
                raise ValueError(f"SHA-256 changed: {source!r} -> {destination!r}")
            verified += 1
        except (OSError, ValueError) as error:
            failures.append(str(error))

    print(f"Moves: {len(moves)}; affected files: {len(affected)}; "
          f"verified files: {verified}; absent sources: {absent}; failures: {len(failures)}")
    for failure in failures:
        print(f"FAIL: {failure}")
    return 1 if failures else 0


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        epilog="Directory mappings include descendants; the most specific source wins. "
               "All original source paths must be absent. Paths may use / or \\. "
               "Exit codes: 0 preserved, 1 verification failures, 2 invalid input. "
               "No files are written, moved, or removed.")
    parser.add_argument("--inventory", required=True, type=Path,
                        help="pre-move JSON: {files: [{path, sha256, bytes, tracked}]}")
    parser.add_argument("--moves", required=True, type=Path,
                        help="JSON array: [{source, destination, kind}]")
    parser.add_argument("--root", type=Path, default=Path(__file__).absolute().parents[3],
                        help="repository root (default: this script's repository)")
    args = parser.parse_args()
    try:
        reject_links(args.root.absolute())
        root = args.root.resolve(strict=True)
        if not root.is_dir():
            raise ValueError("repository root must be a directory")
        return verify(root, read_json(args.inventory), load_moves(read_json(args.moves)))
    except (OSError, ValueError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
