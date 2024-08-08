set -e

apt-get update
xargs --arg-file=packages.txt apt-get --assume-yes install

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 18
apt-get --assume-yes install clangd-18

rm --recursive --force /var/cache/apt/archives /var/lib/apt/lists/*.
