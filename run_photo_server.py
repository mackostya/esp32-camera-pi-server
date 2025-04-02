import threading
import time 
import os
import logging

time_delay = 60*60 # 6
class ScheduleTask(threading.Thread):
    def __init__(self):
        
        logging.basicConfig(
            format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
            level=logging.INFO,
            filename = "log.log"
        )

        super(ScheduleTask, self).__init__(name="PhotoServer")


    def run(self):
        while True:
            logging.info("Taking an image")
            cureent_time = time.strftime("%Y-%m-%d_%Hh%Mm")
            take_image_prompt = f"libcamera-still --width 1536 --height 1024 -o /var/www/html/images/image{cureent_time}.jpg"
            out = os.popen(take_image_prompt).read()
            logging.info(out)
            time.sleep(time_delay)
            
if __name__ == "__main__":
    task = ScheduleTask()
    task.start()
    task.join()