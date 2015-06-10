import logging
import logging.handlers
import traceback
import cx_Oracle

#import Cmd
import Logger
from RemoteClient import *

APP_NAME = "transfer_dev"
LOG_DIR  = '/tmp/'

class TransferDataFilesException(Exception):
    def __init__(self, error):
        self.error = error
    def __str__(self):
        return self.error.encode("utf-8")

class InfoTransferDataFilesException(TransferDataFilesException):
    pass
    
class WarningTransferDataFilesException(TransferDataFilesException):
    pass

class ErrorTransferDataFilesException(TransferDataFilesException):
    pass


class TransferDataFilesScript:
    logger = None
    #cmd = None

    def __init__(self):
        logger = Logger.Logger(APP_NAME, LOG_DIR)
        self.logger = logger.getLogger(level_log=logging.INFO, level_console=logging.DEBUG)
        #self.cmd = Cmd.Cmd(self.logger)

    def run(self, run2p, i, v, u, h, source, c, p, disk, eos):
        self.logger.info("TransferDataFilesScript.run start")

        try:
            rc = RemoteClient()
            rc.setLogger(self.logger)
            #rc.setCmd(self.cmd)
            rc.transferDataFiles(run2p, i, v, u, h, source, c, p, disk, eos)

        except cx_Oracle.DatabaseError, info:
            #traceback.print_stack()
            self.logger.error("sql_error='%s'", str(info).replace("\n", ""))
        
        except(InfoTransferDataFilesException), error:
            self.db.logger.info(error)
        except(InfoRemoteClientException), error:
            self.db.logger.info(error)
        
        except(WarningTransferDataFilesException), error:
            self.logger.warning(error)
        except(WarningRemoteClientException), error:
            self.logger.warning(error)
        
        except(ErrorTransferDataFilesException), error:
            self.logger.error(error)
        except(ErrorRemoteClientException), error:
            self.logger.error(error)
        
        except(Exception), error:
            self.logger.critical("python_error='%s' traceback='%s'", error, traceback.format_exc())

        self.logger.info("TransferDataFilesScript.run stop")
