import os

proc raiseOsErrorIf*(failure:bool) =
    if failure: raiseOsError osLastError()
