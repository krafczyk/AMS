import logging
import logging.handlers

class Logger:
    default_level = logging.INFO
    app_name = None
    logger = None
    log_dir = None
    log_size = 1024*1024*10
    log_count = 50

    def __init__(self, app_name, log_dir):
        self.app_name = app_name
        self.log_dir = log_dir

    def getLogger(self, level_log=None, level_console=None):
        # setup logger
        logger = logging.getLogger(self.app_name)
        logger.setLevel(logging.DEBUG)

        if self.app_name == "transfer_dev":
            # colorize debug, warnings, errors and criticals
            logging.addLevelName(logging.DEBUG,    "\033[33m%s\033[1;0m" % logging.getLevelName(logging.DEBUG))
            logging.addLevelName(logging.WARNING,  "\033[31m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
            logging.addLevelName(logging.ERROR,    "\033[31m%s\033[1;0m" % logging.getLevelName(logging.ERROR))
            logging.addLevelName(logging.CRITICAL, "\033[31m%s\033[1;0m" % logging.getLevelName(logging.CRITICAL))

        # create file handler
        if self.log_dir is None:
            log_file = "/dev/null"
        else:
            log_file = "%s/%s.log" % (self.log_dir, self.app_name)

        file_h = logging.handlers.RotatingFileHandler(
            log_file,
            maxBytes=self.log_size,
            backupCount=self.log_count
        )
        if level_log is None: level_log = logging.ERROR
        file_h.setLevel(level_log)

        # create console handler
        console_h = logging.StreamHandler()
        if level_console is None: level_console = logging.ERROR
        console_h.setLevel(level_console)

        # create email handler
        #mail_h = logging.handlers.SMTPHandler('localhost', '%s@example.com' % self.app_name, ['odemakov@gmail.com'], self.app_name)
        #mail_h.setLevel(logging.ERROR)

        # create formatter and add it to the handlers
        formatter = logging.Formatter('%(asctime)s - %(name)s - pid=%(process)-4s - %(levelname)-8s %(message)s')
        file_h.setFormatter(formatter)
        console_h.setFormatter(formatter)
        #mail_h.setFormatter(formatter)

        # add the handlers to logger
        logger.addHandler(file_h)
        logger.addHandler(console_h)
        #logger.addHandler(mail_h)
        return logger


