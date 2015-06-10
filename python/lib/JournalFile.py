
class JournalFile:
    __file_path = None
    run = 0
    good = 0
    crc = 0
    crc32 = 0
    fevent = 0
    levent = 0
    events = 0
    size = 0
    tfevent = 0
    tlevent = 0
    timestamp = 0
    path = ""
    errors = ""
    tag = ""
    t0 = ""
    t1 = ""
    t2 = ""
    t3 = ""
    f1 = ""
    f2 = ""
    version = ""

    def __init__(self, file_path):
        self.__file_path = file_path
        f = open(self.__file_path, 'r')
        for linea in f.readlines():
            line = linea.split('\n')[0]
            if(line.find("Run")>=0):
                self.run = int(line.split("=")[1])
            elif(line.find("FEvent")>=0):
                self.fevent = int(line.split("=")[1])
            elif(line.find("LEvent")>=0):
                self.levent = int(line.split("=")[1])
            elif(line.find("TFevent")>=0):
                self.tfevent = int(line.split("=")[1])
            elif(line.find("TLevent")>=0):
                self.tlevent = int(line.split("=")[1])
            elif(line.find("Version")>=0):
                self.version = line.split("=")[1]
            elif(line.find("NEvent")>=0):
                self.events = int(line.split("=")[1])
            elif(line.find("NError")>=0):
                self.errors = line.split("=")[1]
            elif(line.find("CRC32=")>=0):
                self.crc32 = int(line.split("=")[1])
            elif(line.find("CRC=")>=0):
                self.crc = int(line.split("=")[1])
            elif(line.find("Size")>=0):
                self.size = int(line.split("=")[1])
            elif(line.find("Timestamp")>=0):
                self.rtime = int(line.split("=")[1])
            elif(line.find("Path")>=0):
                self.path=line.split("=")[1]
            elif(line.find("Tag")>=0):
                self.tag=line.split("=")[1]
            elif(line.find("Type0")>=0):
                self.t0=line.split("=")[1]
            elif(line.find("Type1")>=0):
                self.t1=line.split("=")[1]
            elif(line.find("Type2")>=0):
                self.t2=line.split("=")[1]
            elif(line.find("Type3")>=0):
                self.t3=line.split("=")[1]
            elif(line.find("FileF")>=0):
                self.f1=line.split("=")[1]
            elif(line.find("FileL")>=0):
                self.f2=line.split("=")[1]

        f.close()

    def ok(self):
        if(self.size>0 and (self.crc>0 or self.crc32>0) and self.events>0 and self.tlevent>0 and self.tfevent>0 and self.levent>0 and self.fevent>0 and self.run>0 and self.rtime>0):
            return 1
        else:
            return 0

    @property
    def file_path(self):
        return self.__file_path

