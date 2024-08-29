set -e

git clone https://github.com/serhmarch/ModbusTools.git
cd ModbusTools
git submodule update --init --recursive

mkdir build
cd build
cmake ..
make -j10

cd ../..
mv ModbusTools/build /opt/modbus_tools
chown --recursive developer:developer /opt/modbus_tools
rm -rf ModbusTools

ln --symbolic /opt/modbus_tools/server /usr/bin/mbserver
ln --symbolic /opt/modbus_tools/client /usr/bin/mbclient

# So we will be able to create socat device files there
mkdir --parents /dev/user
chown developer:developer /dev/user
