#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkDialogs.h"
#include <cstdio>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Dialogue d'ouverture de fichier -------------------------------
    NkDialogResult openResult = NkDialogs::OpenFileDialog(
        "*.*", "c3-exo9 - Choisir un fichier a ouvrir");

    if (openResult.confirmed) {
        NkDialogs::OpenMessageBox(
            "Fichier choisi : " + openResult.path,
            "Ouverture - resultat", 0);
    } else {
        // Annulation geree explicitement : on ne touche pas a
        // openResult.path (qui serait vide de toute facon), et le
        // programme continue normalement.
        NkDialogs::OpenMessageBox(
            "Ouverture annulee : aucun fichier choisi.",
            "Ouverture - annulee", 1);
    }

    // 2. Dialogue de sauvegarde de fichier ------------------------------
    NkDialogResult saveResult = NkDialogs::SaveFileDialog(
        "txt", "c3-exo9 - Choisir ou sauvegarder");

    if (saveResult.confirmed) {
        NkDialogs::OpenMessageBox(
            "Chemin de sauvegarde choisi : " + saveResult.path,
            "Sauvegarde - resultat", 0);
    } else {
        NkDialogs::OpenMessageBox(
            "Sauvegarde annulee : aucun chemin choisi.",
            "Sauvegarde - annulee", 1);
    }

    // 3. Selecteur de couleur -------------------------------------------
    NkDialogResult colorResult = NkDialogs::ColorPicker(0xFFFFFFFF);

    if (colorResult.confirmed) {
        char buf[64];
        std::snprintf(buf, sizeof(buf),
            "Couleur choisie (RGBA) : 0x%08X", colorResult.color);
        NkDialogs::OpenMessageBox(buf, "Couleur - resultat", 0);
    } else {
        NkDialogs::OpenMessageBox(
            "Choix de couleur annule.",
            "Couleur - annulee", 1);
    }

    // 4. Boite de message simple -----------------------------------------
    // Pas de valeur de retour a verifier ici : OpenMessageBox() est void,
    // donc pas d'annulation a gerer pour ce dialogue precis.
    NkDialogs::OpenMessageBox(
        "Les quatre dialogues ont ete utilises, y compris en annulant "
        "chacun d'eux, sans plantage du programme.",
        "c3-exo9 - Termine", 0);

    return 0;
}
