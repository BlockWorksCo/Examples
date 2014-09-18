

from nose import with_setup
import subprocess
import shlex



def Run(command):
    """
    """
    commandList     = shlex.split(command)
    print(commandList)
    return subprocess.check_output(commandList, shell=True)





def setup_module():
    """
    """
    out = Run('Build PLATFORM=Linux clean all')
    print(out)



def teardown_module():
    """
    """
    pass




def TestChecksums():
    """
    vagrant ssh -c "cd /HostRoot/c/BlockWorks/Examples/IPStack/Tests && Output/Main ../Traces/HTTPGET.pcap Output.pcap"
    """
    out = Run(' vagrant ssh -c "cd /HostRoot/c/BlockWorks/Examples/IPStack/Tests && Output/Main ../Traces/HTTPGET.pcap Output.pcap" ')
    print(out)
    assert False







