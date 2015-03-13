"""
pure Python wrapper around libreader.so using ctypes
"""

import ctypes

lib = ctypes.cdll.LoadLibrary('libreader.so')
from message_wrap import Message, as_array

class MessageReader:
    """
    Straightforward mapping of C API to Python class.
    """

    def __init__(self, r=None):
        self.r = r or lib.message_reader_new()
        if not self.r:
            raise RuntimeError("could not create MessageReader object")

    def __del__(self):
        try:
            lib.message_reader_delete(self.r)
        except AttributeError:
            pass # lib was already garbage collected

    def reset(self):
        lib.message_reader_reset(self.r)

    def add_buffer(self, buf):
        a = as_array(buf)
        n = len(buf)
        fail = lib.message_reader_add_buffer(self.r, a, n)
        if fail:
            raise RuntimeError("could not add buffer")

    def get_message(self):
        m = lib.message_reader_get_message(self.r)
        return Message(m) if m else None
