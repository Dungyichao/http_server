import io
import picamera
import sys


running = True

if sys.version_info.major >= 3:
    my_stdout = sys.stdout.buffer
else:
    my_stdout = sys.stdout


def streams_iter():
    while running:
        yield my_stdout
        sys.stdout.flush()
           
with picamera.PiCamera(resolution='640x480', framerate=24) as camera:
    #Uncomment the next line to change your Pi's Camera rotation (in degrees)
    #camera.rotation = 90
    try:
        camera.capture_sequence(streams_iter(), format='jpeg', use_video_port= True, thumbnail=None, quality=50)
    except IOError as e:
        sys.stdout.close()
        print('Python Error !! >>>>>>>')
           

    #camera.start_recording(outputs, format='mjpeg')


