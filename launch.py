#!/usr/bin/python3 -u

import os
import shutil
import subprocess
import signal
import sys
from string import ascii_uppercase
from subprocess import Popen

rdbus = None
def signal_handler( sig, frame ):
    if rdbus:
        rdbus.send_signal( signal.SIGINT )

def setupDirectory( runDir, runConfigDir ):
    # Clean run directory
    if os.path.exists( runDir ):
        shutil.rmtree( runDir )

    shutil.copytree( runConfigDir, runDir )

def copyRdbus( rdbusPath, targetDir ):
    if os.path.exists( rdbusPath ):
        shutil.copy( rdbusPath, targetDir )
    else:
        raise Exception( f'Missing rdbus executable at \'${rdbusPath}\'' )

def getConfigFileCount( configDir ):
    counter = 0
    for file in os.listdir( configDir ):
        if file.endswith( ".rdbus.json" ):
            counter +=1
    return counter

def changeDirectory( launchDir ):
    os.chdir( launchDir )

def startTTY( count ):
    if count == 0:
        raise Exception( "No config files found!" )

    processes = []
    for i in range( count ):
        cmd = f'socat -d -d pty,raw,echo=0,link=/dev/user/tty_master_{ ascii_uppercase[ i ] } pty,raw,echo=0,link=/dev/user/tty_slave_{ ascii_uppercase[ i ] }'
        processes.append( Popen( cmd.split() ) )
    return processes

def startModbusServer( configDir = '' ):
    return Popen( [ 'mbserver', '-project', 'mbserver.pjs', '-no-gui' ], stdout = subprocess.DEVNULL )

def runRdbus( configDir = '' ):
    with Popen( [ './rdbus', '--log', 'info', '--stdout' ] ) as proc:
        global rdbus
        rdbus = proc
        rdbus.wait()

# We cannot terminate this script by Ctrl+C, because the parent process then sends
# this signal to all child processes simultaneously. Socat may then close and delete
# it's /dev/* before rdbus exits, causing rdbus to hang on read/write.
signal.signal( signal.SIGTERM, signal_handler )

# Clean run directory first
setupDirectory( 'run/', 'run.config/' )
# Copy executable to target directory
copyRdbus( 'build/Release/src/apps/rdbus', 'run/' )
# Go to target directory
changeDirectory( 'run/' )
# Count config files to create necessary amount of ports
configCount = getConfigFileCount( os.getcwd() )
# Start serial
ttys = startTTY( configCount )
# Start Modbus server
mbServer = startModbusServer()

# Run rdbus
runRdbus()

mbServer.terminate()

for tty in ttys:
    tty.terminate()
