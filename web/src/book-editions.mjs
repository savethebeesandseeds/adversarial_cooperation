export const bookEditions = Object.freeze([
  Object.freeze({
    id: "short",
    title: "Short book",
    pdfName: "Adversarial-Cooperation-Short.pdf",
    description: "Brief reflections on every chapter, with paths into the research companion for the reasoning, demonstrations, and open questions.",
  }),
  Object.freeze({
    id: "companion",
    title: "Research companion",
    pdfName: "Adversarial-Cooperation.pdf",
    description: "The expanded research draft: detailed chapters, algorithms, references, implementation notes, and ideas still awaiting development.",
  }),
]);

export function readingEdition(hash) {
  if (!hash || hash === "#" || hash === "#read") return bookEditions[0];
  return bookEditions.find((edition) => hash === `#read=${edition.id}`) ?? null;
}

export function editionPdfPath(edition) {
  return `./book/${edition.pdfName}`;
}
