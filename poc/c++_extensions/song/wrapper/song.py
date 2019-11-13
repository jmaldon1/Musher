try:
    import cSong
except:
    strErr = "\n\n`cSong` module not found, "
    raise RuntimeError(strErr)


def PrintFunctionalMessage(message):
    cSong.PrintFunctionalMessage(message)


class song():
    def __init__(self, name, length):
        self.songCapsule = cSong.construct(name, length)

    def __delete__(self):
        cSong.delete_object(self.songCapsule)

    def SetName(self, name):
        cSong.SetName(self.songCapsule, name)

    def SetLength(self, length):
        cSong.SetLength(self.songCapsule, length)

    def GetName(self):
        return cSong.GetName(self.songCapsule)

    def GetLength(self):
        return cSong.GetLength(self.songCapsule)

    def PrintMessage(self):
        cSong.PrintMessage(self.songCapsule)
