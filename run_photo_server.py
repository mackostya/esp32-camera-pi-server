import threading
import time 
import os

time_delay = 60*60 # 6
class ScheduleTask(threading.Thread):
    def __init__(self):
        super(ScheduleTask, self).__init__(name="PhotoServer")


    def run(self):
        while True:
            print("Taking an image")
            cureent_time = time.strftime("%Y-%m-%d_%Hh%Mm")
            take_image_prompt = f"libcamera-still --width 1536 --height 1024 -o /var/www/html/images/image{cureent_time}.jpg"
            os.system(take_image_prompt)
            time.sleep(10)
            
if __name__ == "__main__":
    task = ScheduleTask()
    task.start()
    task.join()