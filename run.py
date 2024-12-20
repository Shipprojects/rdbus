#!/usr/bin/python3 -u

import os
import shutil
import subprocess
import signal
import time
from argparse import ArgumentParser
from string import ascii_uppercase
from subprocess import Popen

parser = ArgumentParser( description = 'Launch rdbus locally with predefined modbus server and serial device emulation. '
                                       'Unknown parameters will be passed to rdbus.' )
parser.add_argument( '--no-modbus', action = 'store_true', help = 'do not start modbus slave (for cases when it is already running)' )
parser.add_argument( '--no-nmea', action = 'store_true', help = 'do not start nmea talker (for cases when it is already running)' )
parser.add_argument( '--config-dir', default = 'run.config', help = 'directory that contains all necessary config files (default - run.config/)' )

args, rdbusArgs = parser.parse_known_args()

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
        cmd = f'socat -d -d pty,raw,echo=0,link=/home/developer/dev/tty_master_{ ascii_uppercase[ i ] } pty,raw,echo=0,link=/home/developer/dev/tty_slave_{ ascii_uppercase[ i ] }'
        processes.append( Popen( cmd.split() ) )

    # Socat needs some time to initialize
    time.sleep( 1 )

    return processes

def startModbusServer():
    return Popen( [ 'mbserver', '-project', 'mbserver.pjs' ], stdout = subprocess.DEVNULL )

def startNMEATalker():
    return Popen( [ 'nmeasimulator', '--no-sandbox' ], stdout = subprocess.DEVNULL )

def runRdbus():
    with Popen( [ './rdbus' ] + rdbusArgs ) as proc:
        global rdbus
        rdbus = proc
        rdbus.wait()

# We cannot terminate this script by Ctrl+C, because the parent process then sends
# this signal to all child processes simultaneously. Socat may then close and delete
# it's dev/* before rdbus exits, causing rdbus to hang on read/write.
signal.signal( signal.SIGTERM, signal_handler )

# Clean run directory first
setupDirectory( 'run/', args.config_dir )
# Copy executable to target directory
copyRdbus( 'build/Release/src/apps/rdbus', 'run/' )
# Go to target directory
changeDirectory( 'run/' )
# Count config files to create necessary amount of ports
configCount = getConfigFileCount( os.getcwd() )
# Start serial
ttys = startTTY( configCount )
# Start Modbus server
mbServer = None
if not args.no_modbus:
    mbServer = startModbusServer()
# Start NMEA talker
nmeaTalker = None
if not args.no_nmea:
    nmeaTalker = startNMEATalker()

# Run rdbus
runRdbus()

if nmeaTalker is not None:
    nmeaTalker.terminate()

if mbServer is not None:
    mbServer.terminate()

for tty in ttys:
    tty.terminate()
