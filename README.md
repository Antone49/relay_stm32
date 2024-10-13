# Compilation 🚀

cmake . -B _build

cmake --build _build --target install -j16

# Flash

./script/flash.sh


## Debogage depuis le docker

Pour utiliser le debogage, vous devez lancer le script openocd_gdbserver.bat sur le PC host (Windows) qui va faire la liaison entre un port (3333) et le stlink.

Docker ---------> PC Host ---------> Stlink ---------> Stm

Vous pouvez ensuite utiliser le debugger de vscode. Le script .vscode/launch.json va effectuer la configuration du debugger
