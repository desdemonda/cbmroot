#!/usr/bin/env python

import unittest
from message_wrap import Message

class MessageTestCase(unittest.TestCase):
    def setUp(self):
        self.m = Message()

class MessageNew(MessageTestCase):
    """
    Test properties of a newly created Message.

    Most other test cases inherit their tests from here.
    """
    def test_complete(self):
        self.assertFalse(self.m.is_complete)

    def test_valid(self):
        self.assertFalse(self.m.is_valid)

    def test_hit(self):
        self.assertFalse(self.m.is_hit)

    def test_hit_aborted(self):
        self.assertFalse(self.m.is_hit_aborted)

    def test_buffer_overflow(self):
        self.assertFalse(self.m.is_buffer_overflow)

    def test_epoch_marker(self):
        self.assertFalse(self.m.is_epoch_marker)

    def test_epoch_out_of_sync(self):
        self.assertFalse(self.m.is_epoch_out_of_sync)

    def test_info(self):
        self.assertFalse(self.m.is_info)

    def test_samples(self):
        self.assertIsNone(self.m.samples)

class MessageNewReset(MessageNew):
    """
    Reset must not change a new message.
    """
    def setUp(self):
        self.m = Message()
        self.m.reset()

class MessageValidReset(MessageNew):
    """
    Reset of a valid message must change it to a new one.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
            0xB000
        ])
        self.m.reset()

#--------------------------------------------------------------------

class MessageReadFromBuffer(MessageTestCase):
    """
    Test behaviour of read_from_buffer().
    """
    def setUp(self):
        self.m = Message()

    def test_empty_buffer(self):
        n = self.m.read_from_buffer([])
        self.assertEqual(n, 0)

    def test_read_until_end(self):
        n = self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
            0xB000,
            0x8000,
            0x9000,
        ])
        self.assertEqual(n, 4)

    def test_complete_message(self):
        self.m.read_from_buffer([
            0x9000,
            0xA000,
            0xB000,
        ])
        self.assertTrue(self.m.is_complete)

    def test_incomplete_message(self):
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
        ])
        self.assertFalse(self.m.is_complete)

    def test_complete_message(self):
        self.m.read_from_buffer([
            0x9000,
            0xA000,
            0xB000,
        ])
        self.assertTrue(self.m.is_complete)

    def test_reuse_message(self):
        self.m.read_from_buffer([
            0x8000,
            0x9000,
        ])
        self.m.read_from_buffer([
            0xA000,
            0xB000,
        ])
        self.assertTrue(self.m.is_valid)

    def test_start_resets_message(self):
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
            0xB000,
        ])
        self.assertTrue(self.m.is_valid)
        self.m.read_from_buffer([
            0x8000,
        ])
        self.assertFalse(self.m.is_valid)

#--------------------------------------------------------------------

class MessageHitBase(MessageNew):
    """
    Test properties of a simple hit message.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
            0xB000
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_hit(self):
        self.assertTrue(self.m.is_hit)

    def test_samples(self):
        self.assertEqual(self.m.samples, [])

class MessageHitValidSamples(MessageHitBase):
    """
    Test values contained in a hit message where the samples are valid.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8ABC,
            0x9DEF,
            0xA008,
            0x0400,
            0xB0B5
        ])

    def test_group_id(self):
        self.assertEqual(self.m.group_id, 0xAB)

    def test_channel_id(self):
        self.assertEqual(self.m.channel_id, 0xC)

    def test_timestamp(self):
        self.assertEqual(self.m.timestamp, 0xDEF)

    def test_stop_type(self):
        self.assertEqual(self.m.stop_type, 0x5)

    def test_hit_type(self):
        self.assertEqual(self.m.hit_type, 0x3)

    def test_samples(self):
        self.assertEqual(self.m.samples, [1, 2])

class MessageHitInvalidSamples(MessageHitBase):
    """
    Hit message with fewer contained samples than indicated.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
            0xB100
        ])

    def test_samples(self):
        self.assertIsNone(self.m.samples)

#--------------------------------------------------------------------

class MessageHitAbortedDisabled(MessageNew):
    """
    Hit message aborted because channel disabled.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xF060
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_hit_aborted(self):
        self.assertTrue(self.m.is_hit_aborted)

    def test_channel_id(self):
        self.assertEqual(self.m.channel_id, 6)

    def test_info_type(self):
        self.assertEqual(self.m.info_type, 0)

class MessageHitAbortedCorruption(MessageNew):
    """
    Hit message aborted because corruption in message builder.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8000,
            0x9000,
            0xA000,
            0xF460
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_hit_aborted(self):
        self.assertTrue(self.m.is_hit_aborted)

    def test_channel_id(self):
        self.assertEqual(self.m.channel_id, 6)

    def test_info_type(self):
        self.assertEqual(self.m.info_type, 4)

#--------------------------------------------------------------------

class MessageBufferOverflowCount(MessageNew):
    """
    Buffer overflow count message.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8ABC,
            0x9DEF,
            0xC012
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_buffer_overflow(self):
        self.assertTrue(self.m.is_buffer_overflow)

    def test_group_id(self):
        self.assertEqual(self.m.group_id, 0xAB)

    def test_channel_id(self):
        self.assertEqual(self.m.channel_id, 0xC)

    def test_timestamp(self):
        self.assertEqual(self.m.timestamp, 0xDEF)

    def test_buffer_overflow_count(self):
        self.assertEqual(self.m.buffer_overflow_count, 0x12)

#--------------------------------------------------------------------

class MessageEpochMarker(MessageNew):
    """
    Normal epoch marker.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8ABC,
            0xDDEF,
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_epoch_marker(self):
        self.assertTrue(self.m.is_epoch_marker)

    def test_group_id(self):
        self.assertEqual(self.m.group_id, 0xAB)

    def test_epoch(self):
        self.assertEqual(self.m.epoch_count, 0xDEF)

class MessageEpochOutOfSync(MessageNew):
    """
    "out of sync" epoch marker.
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0x8ABC,
            0xF6EF,
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_epoch_out_of_sync(self):
        self.assertTrue(self.m.is_epoch_out_of_sync)

    def test_group_id(self):
        self.assertEqual(self.m.group_id, 0xAB)

    def test_epoch(self):
        self.assertEqual(self.m.epoch_count, 0xEF)

    def test_info_type(self):
        self.assertEqual(self.m.info_type, 6)

#--------------------------------------------------------------------

class MessageInfoNGT(MessageNew):
    """
    Info message "NGT".
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0xF1AB,
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_info(self):
        self.assertTrue(self.m.is_info)

    def test_info_type(self):
        self.assertEqual(self.m.info_type, 1)

    def test_channel_id(self):
        self.assertEqual(self.m.channel_id, 0xA)

class MessageInfoNBE(MessageNew):
    """
    Info message "NBE".
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0xF3AB,
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_info(self):
        self.assertTrue(self.m.is_info)

    def test_info_type(self):
        self.assertEqual(self.m.info_type, 3)

    def test_channel_id(self):
        self.assertEqual(self.m.channel_id, 0xA)

class MessageInfoNRT(MessageNew):
    """
    Info message "NRT".
    """
    def setUp(self):
        self.m = Message()
        self.m.read_from_buffer([
            0xF2AB,
        ])

    def test_complete(self):
        self.assertTrue(self.m.is_complete)

    def test_valid(self):
        self.assertTrue(self.m.is_valid)

    def test_info(self):
        self.assertTrue(self.m.is_info)

    def test_info_type(self):
        self.assertEqual(self.m.info_type, 2)

#--------------------------------------------------------------------

if  __name__=='__main__':
    unittest.main()
