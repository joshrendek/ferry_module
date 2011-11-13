#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");

//Function prototypes
void init_vars(void);
void ferry_loop(void);
void print_curr(void);
void begin_ferry(void);


/* Extern system call stub declarations */
extern long (*STUB_ferry_start)(int start_int);
extern long (*STUB_ferry_request)(int request_int);
extern long (*STUB_ferry_stop)(int stop_int);

// Global Vars

struct bank {
  int queen;
  int nobles;
  int peasants;
};

struct ferry {
  int queen;
  int nobles;
  int peasants;
  int compartments;
  int curr_bank;	//0 = north, 1 = south, 2 = in transit
};

struct bank north_bank;
struct bank south_bank;
struct ferry curr_ferry;
int curr_ferry_passengers;
int total_ferried;
struct task_struct *ferry_thread;
struct mutex locker;

long my_ferry_start(int start_int)
{
  printk("%s: Ferry started!\n", __FUNCTION__);

  init_vars();	//set all of the variables in the structs to 0

  //begin ferry thread
  ferry_thread = kthread_run(begin_ferry, NULL, "ferry_thread");

  return start_int;
}

void begin_ferry()
{

  mutex_init(&locker);
  int i = 5;
  while(i > 0)
  {
    ferry_loop();
    i--;
  }
  printk("%s: **THREAD STOPPING**\n", __FUNCTION__);

}

void print_curr()
{
  printk("%s: North: Queen: %d\n", __FUNCTION__, north_bank.queen);
  printk("%s: North: Nobles: %d\n", __FUNCTION__, north_bank.nobles);
  printk("%s: North: Peasants: %d\n", __FUNCTION__, north_bank.peasants);
  printk("%s: South: Queen: %d\n", __FUNCTION__, south_bank.queen);
  printk("%s: South: Nobles: %d\n", __FUNCTION__, south_bank.nobles);
  printk("%s: South: Peasants: %d\n", __FUNCTION__, south_bank.peasants);
}

void release_passengers()
{
  curr_ferry.queen = 0;
  curr_ferry.nobles = 0;
  curr_ferry.peasants = 0;
  curr_ferry.compartments = 0;
  curr_ferry_passengers = 0;
}

void ferry_loop()
{
  //arrive at north_bank
  curr_ferry.curr_bank = 0;
  printk("%s: Ferry arrived at north bank\n", __FUNCTION__);
  //lock
  mutex_lock(&locker);

  //release all passengers
  release_passengers();

  //print_curr();

  //load passengers
  if(north_bank.queen == 1)
  {
    printk("%s: Picking up north queen\n", __FUNCTION__);
    curr_ferry.queen++;
    north_bank.queen--;
    curr_ferry.compartments+=5;
    curr_ferry_passengers++;
    total_ferried++;
  }
  if(north_bank.nobles > 0)
  {
    printk("%s: Picking up north nobles: %d\n", __FUNCTION__, north_bank.nobles);
    while((curr_ferry.compartments + 3) < 15 && north_bank.nobles > 0)
    {
      curr_ferry.nobles++;
      north_bank.nobles--;
      curr_ferry.compartments+=3;
      curr_ferry_passengers++;
      total_ferried++;
    }
  }
  if(north_bank.peasants > 0)
  {
    printk("%s: Picking up north peasants: %d\n", __FUNCTION__, north_bank.peasants);
    while(curr_ferry.compartments < 15 && north_bank.peasants > 0)
    {
      curr_ferry.peasants++;
      north_bank.peasants--;
      curr_ferry.compartments++;
      curr_ferry_passengers++;
      total_ferried++;
    }
  }
  /*printk("%s: Ferry: Queen: %d\n", __FUNCTION__, curr_ferry.queen);
    printk("%s: Ferry: Nobles: %d\n", __FUNCTION__, curr_ferry.nobles);
    printk("%s: Ferry: Peasants: %d\n", __FUNCTION__, curr_ferry.peasants);*/

  printk("%s: **IN TRANSIT**\n", __FUNCTION__);
  msleep(2000);

  //printk("%s: Releasing lock, in transit\n", __FUNCTION__);

  //releaseLock()
  mutex_unlock(&locker);


  //NEED TO SLEEP THREAD OR SOME SHIT
  curr_ferry.curr_bank = 2; //in transit

  /*while(travelling)
    {
    if queen appears on a bank
    go to that bank and reload
    }*/
  //arrive at south_bank
  printk("%s: Arrived at south bank\n", __FUNCTION__);
  curr_ferry.curr_bank = 1;

  //lock
  mutex_lock(&locker);

  //release all passengers
  release_passengers();

  //print_curr();

  //load passengers
  if(south_bank.queen == 1)
  {
    printk("%s: Picking up south queen\n", __FUNCTION__);
    curr_ferry.queen++;
    south_bank.queen--;
    curr_ferry.compartments+=5;
    curr_ferry_passengers++;
    total_ferried++;
  }
  if(south_bank.nobles > 0)
  {
    printk("%s: Picking up south nobles: %d\n", __FUNCTION__, south_bank.nobles);
    while((curr_ferry.compartments + 3) <= 15 && south_bank.nobles > 0)
    {
      curr_ferry.nobles++;
      south_bank.nobles--;
      curr_ferry.compartments+=3;
      curr_ferry_passengers++;
      total_ferried++;
    }
  }
  if(south_bank.peasants > 0)
  {
    printk("%s: Picking up south peasants: %d\n", __FUNCTION__, south_bank.peasants);
    while(curr_ferry.compartments < 15 && south_bank.peasants > 0)
    {
      curr_ferry.peasants++;
      south_bank.peasants--;
      curr_ferry.compartments++;
      curr_ferry_passengers++;
      total_ferried++;
    }
  }
  /*printk("%s: Ferry: Queen: %d\n", __FUNCTION__, curr_ferry.queen);
    printk("%s: Ferry: Nobles: %d\n", __FUNCTION__, curr_ferry.nobles);
    printk("%s: Ferry: Peasants: %d\n", __FUNCTION__, curr_ferry.peasants);*/

  printk("%s: **IN TRANSIT**\n", __FUNCTION__);
  msleep(2000);

  //release lock
  mutex_unlock(&locker);

  //print_curr();
}

void init_vars()
{
  north_bank.queen = 1;
  north_bank.nobles = 9;
  north_bank.peasants = 16;
  south_bank.queen = 1;
  south_bank.nobles = 12;
  south_bank.peasants = 10;
  curr_ferry.queen = 0;
  curr_ferry.nobles = 0;
  curr_ferry.peasants = 0;
  curr_ferry.curr_bank = 0;	//starts at north
  curr_ferry.compartments = 0;
  curr_ferry_passengers = 0;
  total_ferried = 0;
}

long my_ferry_request(int request_int)
{
  printk("%s: Ferry request %i\n", __FUNCTION__, request_int);

  char start_bank = 'n';
  mutex_lock(&locker);
  if(request_int == 0)
  {
    if(start_bank == 'n')
    {
      if(north_bank.queen == 1)
      {
        printk("%s: Failed to issue queen request: too many queens\n", __FUNCTION__);
        mutex_unlock(&locker);
        return 1;
      }
      else
        north_bank.queen++;
    }
    else if(start_bank == 's')
    {
      if(south_bank.queen == 1)
      {
        printk("%s: Failed to issue queen request: too many queens\n", __FUNCTION__);
        mutex_unlock(&locker);
        return 1;
      }
      else
        south_bank.queen++;
    }
  }
  else if(request_int == 1)
  {
    if(start_bank == 'n')
      north_bank.nobles++;
    else if(start_bank == 's')
      south_bank.nobles++;
  }
  else if(request_int == 2)
  {
    if(start_bank == 'n')
      north_bank.peasants++;
    else if(start_bank == 's')
      south_bank.peasants++;
  }
  else{
    mutex_unlock(&locker);
    return 1;
  }
  mutex_unlock(&locker);
  return 0;
}

long my_ferry_stop(int stop_int)
{
  printk("%s: Ferry stop %i\n", __FUNCTION__, stop_int);
  //kthread_stop(ferry_thread);
  return stop_int;
}

my_module_init() {
  STUB_ferry_start=&(my_ferry_start);
  STUB_ferry_request=&(my_ferry_request);
  STUB_ferry_stop=&(my_ferry_stop);
  return 0;
}

my_module_exit() {
  STUB_ferry_start=NULL;
  STUB_ferry_request=NULL;
  STUB_ferry_stop=NULL;
}

module_init(my_module_init);
module_exit(my_module_exit);
