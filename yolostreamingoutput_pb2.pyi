from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class YoloReply(_message.Message):
    __slots__ = ["detection"]
    DETECTION_FIELD_NUMBER: _ClassVar[int]
    detection: str
    def __init__(self, detection: _Optional[str] = ...) -> None: ...

class YoloRequest(_message.Message):
    __slots__ = ["message"]
    MESSAGE_FIELD_NUMBER: _ClassVar[int]
    message: str
    def __init__(self, message: _Optional[str] = ...) -> None: ...
