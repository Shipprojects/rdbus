set -e

wget https://github.com/panaaj/nmeasimulator/releases/download/v1.6.1/nmeasimulator_1.6.1_amd64.deb
dpkg --install nmeasimulator_1.6.1_amd64.deb

# This is a hack to make nmeasimulator preconfigured. The program stores all settings in a 000003.log file
# as you click on various options there.
mkdir --parents '/home/developer/.config/NMEASimulator/Local Storage/leveldb/'
cp 000003.log '/home/developer/.config/NMEASimulator/Local Storage/leveldb/'
chown developer:developer '/home/developer/.config/NMEASimulator/Local Storage/leveldb/000003.log'

chown developer:developer '/home/developer/.config' '/home/developer/.config/NMEASimulator' '/home/developer/.config/NMEASimulator' \
                          '/home/developer/.config/NMEASimulator/Local Storage/' '/home/developer/.config/NMEASimulator/Local Storage/leveldb/' \
                          '/home/developer/.config/NMEASimulator/Local Storage/leveldb/000003.log'