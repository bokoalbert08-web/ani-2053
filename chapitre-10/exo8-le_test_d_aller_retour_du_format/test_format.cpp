#include "NkRef/NkRefFile.h"
#include <iostream>

using namespace nkref;

int main() {
    NkRefBoard board;

    NkRefItem image1;
    image1.pos = {10.0f, 20.0f};
    image1.scale = 1.5f;
    image1.rotationDeg = 15.0f;
    image1.opacity = 0.8f;
    image1.texW = 64;
    image1.texH = 48;
    board.items.PushBack(image1);

    NkRefItem image2;
    image2.pos = {-30.0f, 12.0f};
    image2.scale = 2.0f;
    image2.texW = 128;
    image2.texH = 96;
    board.items.PushBack(image2);

    const uint8 source1[] = {0x89, 0x50};
    const uint8 source2[] = {0xFF};

    NkRefFileImage images[2] = {
        {source1, sizeof(source1)},
        {source2, sizeof(source2)}
    };

    NkRefStroke stroke;
    stroke.r = 255;
    stroke.g = 120;
    stroke.b = 40;
    stroke.widthWorld = 2.5f;
    stroke.points.PushBack({1.0f, 2.0f});
    stroke.points.PushBack({3.0f, 4.0f});
    board.strokes.PushBack(stroke);

    NkRefView view;
    view.center = {125.0f, -75.0f};
    view.zoom = 2.5f;

    NkVector<uint8> original;
    NkRefSerialize(original, board, images, view, true, true);

    NkVector<NkRefLoadedItem> loadedItems;
    NkVector<NkRefStroke> loadedStrokes;
    NkRefView loadedView;
    bool dark = false;
    bool showGrid = false;

    const bool readOK = NkRefDeserialize(
        original.Data(), original.Size(),
        loadedItems, loadedStrokes, loadedView, dark, showGrid
    );

    bool identical = false;

    if (readOK && loadedItems.Size() == 2) {
        NkRefBoard rebuilt;

        for (usize i = 0; i < loadedItems.Size(); ++i)
            rebuilt.items.PushBack(loadedItems[i].item);

        for (usize i = 0; i < loadedStrokes.Size(); ++i)
            rebuilt.strokes.PushBack(loadedStrokes[i]);

        NkRefFileImage loadedImages[2] = {
            {loadedItems[0].src.Data(), loadedItems[0].src.Size()},
            {loadedItems[1].src.Data(), loadedItems[1].src.Size()}
        };

        NkVector<uint8> second;
        NkRefSerialize(second, rebuilt, loadedImages, loadedView, dark, showGrid);

        identical = original.Size() == second.Size();

        if (identical) {
            for (usize i = 0; i < original.Size(); ++i) {
                if (original[i] != second[i]) {
                    identical = false;
                    break;
                }
            }
        }
    }

    std::cout << "octets : " << original.Size()
              << ", relu : " << (readOK ? "OUI" : "NON")
              << ", identiques : " << (identical ? "OUI" : "NON")
              << '\n';

    return 0;
}
