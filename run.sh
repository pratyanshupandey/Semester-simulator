make simulator
make psne_calculator
./simulator
./psne_calculator < game.nfg > psne_output.txt
echo "PSNE calculated"
python3 maximal_psne.py