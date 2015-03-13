"""
pure Python wrapper around libmessage.so using ctypes
"""

import ctypes

lib = ctypes.cdll.LoadLibrary('libmessage.so')

def as_array(words, dtype=ctypes.c_uint16):
    return (len(words) * dtype)(*words)

class Message(object):
    """
    Straightforward mapping of C API to Python class.
    """

    #---- create, destroy, fill ---------------------------

    def __init__(self, m=None):
        self.m = m or lib.message_new()
        if not self.m:
            raise RuntimeError("could not create Message object")

    def __del__(self):
        try:
            lib.message_delete(self.m)
        except AttributeError:
            pass # lib was garbage collected before the last Message object

    def reset(self):
        lib.message_reset(self.m)

    def read_from_buffer(self, buf):
        a = as_array(buf) # keep this reference until the function returns
        return lib.message_read_from_buffer(self.m, a, len(buf))

    #---- query status/type -------------------------------

    @property
    def is_complete(self):
        return bool(lib.message_is_complete(self.m))

    @property
    def is_valid(self):
        return bool(lib.message_is_valid(self.m))

    @property
    def is_hit(self):
        return bool(lib.message_is_hit(self.m))

    @property
    def is_hit_aborted(self):
        return bool(lib.message_is_hit_aborted(self.m))

    @property
    def is_buffer_overflow(self):
        return bool(lib.message_is_buffer_overflow(self.m))

    @property
    def is_epoch_marker(self):
        return bool(lib.message_is_epoch_marker(self.m))

    @property
    def is_epoch_out_of_sync(self):
        return bool(lib.message_is_epoch_out_of_sync(self.m))

    @property
    def is_info(self):
        return bool(lib.message_is_info(self.m))

    #---- access data -------------------------------------

    @property
    def group_id(self):
        return lib.message_get_group_id(self.m)

    @property
    def channel_id(self):
        return lib.message_get_channel_id(self.m)

    @property
    def timestamp(self):
        return lib.message_get_timestamp(self.m)

    @property
    def samples(self):
        s = lib.message_get_samples(self.m)
        if not s:
            return None
        n = lib.message_get_num_samples(self.m)
        p = ctypes.POINTER(n * ctypes.c_int16)
        return list(ctypes.cast(s, p).contents)

    @property
    def hit_type(self):
        return lib.message_get_hit_type(self.m)

    @property
    def stop_type(self):
        return lib.message_get_stop_type(self.m)

    @property
    def buffer_overflow_count(self):
        return lib.message_get_buffer_overflow_count(self.m)

    @property
    def epoch_count(self):
        return lib.message_get_epoch_count(self.m)

    @property
    def info_type(self):
        return lib.message_get_info_type(self.m)
