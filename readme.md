#A dragon head build with Arduino

We built a dragon for the christmas parade 2013, it is quite a big one around 7,8 meters, with a head driven by 3 servos and 4 leds, which are controllered by an arduion processor.

##Actions:

* Two noses powered by two leds, will fading in red color repeatly
* Two eyes powered by two leds, steady white color
* Two ears are driven by one continus servo, 
* two eyebrows are driven by two step servo, which open/close the eyebrows
* two push button are installed in the handle, when pressed, the ears will move faster, one eyebrow will stay open and the other will wink faster, depend witch button are pressed.

##File list:
* **dragon.fzz**  the source file for schematic
* **dragon_schem** pdf of the schematic
* **connectors.xlxs** connectors drawing in excel format
* **connectors.pdf** connectors drawing in pdf format
* **dragon/dragon.ico** the source file
* **dragon/schedule.*** the source file for scheduler
* **dragon.test/** the new version to be developed, see to do

##Scheduler:
Starting Arduino programing is too easy, but once the logic get more complicated, it is hard to control those leds and servos in the **loop** function, a scheduler is required.

My scheduler is based on [this one](https://github.com/blanboom/Arduino-Task-Scheduler), but made several changes

* Use timer0 instead of timer1, because timer1 is used by servo library. It would be better to use timer2, but I can't get it work, it seems timer2 is used by system somewhere

* add an **editTask** function to change the setting of a task, mostly the period/frequences of the task. (when press button, make the task run more frequently)

* There are several other changes I would like to make in the next versions

##Push button Schmitt trigger 
the push button confused me for a while, when you push or release the button, the state will change several times instead of one. the hardware solution is add a Schmitt trigger after the push button, the software soluction is to read the state of the button and wait and read again, until it is steady

##millis()
millis() is using timer0, since I changed timer0, it is no longer working. I tried to add an counter in the Sch class, but couldn't get it working. so instead, I created a timer job, run every 1ms, this become my millis function

##Push button hardware interrupt
It is more natural/advanced to use hardware interrupt, but I cannot get it work. program hangs spreadcally for no reason, so I give up and byack to pull button state

##Eye blink
Eye blink seems simple but not really, initial I close the eye lid wait 500ms and open it and wait 500ms, it works but the dragon seems sleepy. thank about how we blink our eye lids. It is close, wait 200ms, open, wait 1000ms.

##To do
###Hardware
Right now we have two set of battery, a 9V battery to power arduino board and leds, a 6V rechageable battery for the servos. 
I would like to use only one set of battery
###Software
* use hardware interrupt for pushbutton
* I couldn't find a free/open source software emulator for arduino, quite a surprise, you would think they should be avavable, It will help a lot when the program gets more complicated.
* more changes for the schedule class
 
  ** change the task function to **void (*pTask)(int);**, ie, add a int parameter to the function, this parameter can be an integer or a pointer to a structure. also add the parameter into the **task** structure, this way, I can schedule multiple similar tasks by using one single function task function with parameters 
  
  ** add a tm / counter in the Sch class, use it to replace timer job and mills function. I tried but cannot get it to work
  
  **Add an enable/disable function to pause the task, right now I use the one time task to blink the eye lid, so every time I have to recreate a one time job for this, would be better to have a persistent job but pause it.
  























