# Compilation 🚀

cmake . -B _build

cmake --build _build --target install -j16

# Flash

./script/flash.sh


## Debogage depuis le docker

Pour utiliser le debogage, vous devez lancer le script openocd_gdbserver.bat sur le PC host (Windows) qui va faire la liaison entre un port (3333) et le stlink.

Docker ---------> PC Host ---------> Stlink ---------> Stm

Vous pouvez ensuite utiliser le debugger de vscode. Le script .vscode/launch.json va effectuer la configuration du debugger


# Bluetooth
code 1234 ou Pas de mot passe, mais si 6 caracteres

Mettre EN à 3.3V au demarrage pour entrer dans la configuration
Pour acces aux commandes AT, mettre la pin en à high et communication uart 38400
Ne pas oublier 0x0D 0x0A (\r\n) a la fin des commandes AT
