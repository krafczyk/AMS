import subprocess
from subprocess import CalledProcessError

# functions for python 2.6
def check_output(*popenargs, **kwargs):
    r"""Run command with arguments and return its output as a byte string.
    Backported from Python 2.7 as it's implemented as pure python on stdlib.
    >>> check_output(['/usr/bin/python', '--version'])
    Python 2.6.2
    """
    process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
    output, unused_err = process.communicate()
    retcode = process.poll()
    if retcode:
        cmd = kwargs.get("args")
        if cmd is None:
            cmd = popenargs[0]
        error = subprocess.CalledProcessError(retcode, cmd)
        error.output = output
        raise error
    return output

def check_call(*popenargs, **kwargs):
    """Run command with arguments.  Wait for command to complete.  If
    the exit code was zero then return, otherwise raise
    CalledProcessError.  The CalledProcessError object will have the
    return code in the returncode attribute.

    The arguments are the same as for the Popen constructor.  Example:

    check_call(["ls", "-l"])
    """
    retcode = subprocess.call(*popenargs, **kwargs)
    if retcode:
        cmd = kwargs.get("args")
        if cmd is None:
            cmd = popenargs[0]
        raise subprocess.CalledProcessError(retcode, cmd)

    return 0


class Cmd:
    def __init__(self, logger):
        self.logger = logger

    def check_output(self, *popenargs, **kwargs):
        self.logger.debug("CMD='%s'", " ".join(popenargs[0]))
        return check_output(*popenargs, **kwargs)

    def check_call(self, *popenargs, **kwargs):
        self.logger.debug("CMD='%s'", " ".join(popenargs[0]))
        return check_call(*popenargs, **kwargs)

    def check_output_save(self, *popenargs, **kwargs):
        result = 0
        try:
            #result = check_output(*popenargs, **kwargs)
            pass
        except CalledProcessError, error:
            result = error.returncode

        self.logger.debug("CMD='%s', return code=%d", " ".join(popenargs[0]), result)
        return result

    def check_call_save(self, *popenargs, **kwargs):
        result = 0
        try:
            #result = check_call(*popenargs, **kwargs)
            pass
        except CalledProcessError, error:
            result = error.returncode

        self.logger.debug("CMD='%s', return code=%d", " ".join(popenargs[0]), result)
        return result

