#!/bin/sh

cp ~/.config/GIMP/2.10/palettes/SkaneRoyale.gpl . &&
        python GIMPPaletteParse.py SkaneRoyale.gpl temp &&
        python File2Palette.py temp ../src/resources/color_palette &&
        rm temp
