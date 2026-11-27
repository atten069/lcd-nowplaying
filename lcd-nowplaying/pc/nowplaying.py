import asyncio
import unicodedata
import serial
from winrt.windows.media.control import (
    GlobalSystemMediaTransportControlsSessionManager as MediaManager,
)

PORT = "COM3"
BAUD = 9600

def clean(text):
    # Das Display kennt keine Umlaute
    for old, new in (("ä", "ae"), ("ö", "oe"), ("ü", "ue"),
                     ("Ä", "Ae"), ("Ö", "Oe"), ("Ü", "Ue"), ("ß", "ss")):
        text = text.replace(old, new)
    text = unicodedata.normalize("NFKD", text).encode("ascii", "ignore").decode()
    return text.replace("|", "-")

async def get_current_song():
    manager = await MediaManager.request_async()
    session = manager.get_current_session()
    if session is None:
        return None
    info = await session.try_get_media_properties_async()
    return info.artist, info.title

async def main():
    arduino = serial.Serial(PORT, BAUD)
    await asyncio.sleep(2)  # der Mega startet neu, sobald der Port geöffnet wird
    last = None
    while True:
        song = await get_current_song()
        if song and song != last:
            artist, title = song
            arduino.write(f"{clean(artist)}|{clean(title)}\n".encode())
            last = song
        await asyncio.sleep(1)

asyncio.run(main()) #python pc/nowplaying.py