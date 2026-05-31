#!/bin/bash

mapfile -t connections < <(nmcli -t -f NAME connection show)

if [ ${#connections[@]} -eq 0 ]; then
    exit 1
fi

echo "Available connections:"
prawdopodobne_pol=-1

for i in "${!connections[@]}"; do
    echo "$((i + 1)). ${connections[i]}"
    [[ "${connections[i]}" == "eduroam" ]] && prawdopodobne_pol=$i
done

if [[ $prawdopodobne_pol -ne -1 ]]; then
    echo "Found 'eduroam' connection at number $((prawdopodobne_pol + 1))"
    read -p "Continue? t/n?
" potwierdzenie
    [[ "$potwierdzenie" =~ ^[Tt]$ ]] && wybrane="${connections[prawdopodobne_pol]}"
fi

while [[ -z "$wybrane" ]]; do
    read -p "Choose number: " wybor
    if [[ "$wybor" =~ ^[0-9]+$ ]] && [ "$wybor" -ge 1 ] && [ "$wybor" -le "${#connections[@]}" ]; then
        wybrane="${connections[$((wybor - 1))]}"
    else
        echo "Choose number: "
    fi
done

echo "Wybrano: $wybrane"

# SPRAWDZENIE: Modyfikuj tylko jeśli sieć to eduroam LUB ma aktywną sekcję 802-1x
if [[ "$wybrane" == "eduroam" ]] || nmcli connection show "$wybrane" | grep -q "^802-1x"; then
    if nmcli connection modify "$wybrane" 802-1x.phase1-auth-flags 32; then
        echo "Restarting NetworkManager..."
        sudo systemctl restart NetworkManager.service && echo "Done!"
    else
        echo "Failed to modify 802-1x settings."
        exit 1
    fi
else
    echo "This network does not use 802.1x. Make sure you chose eduroam network. I'm skipping modifying the flags... Try using eduroam configuration assistant tool from https://cat.eduroam.org/# first."
    echo "Done!"
fi
