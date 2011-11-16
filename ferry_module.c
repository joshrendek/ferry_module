/* ---------------------------------------
	Greg Lombardo && Josh Rendek
       Ferry Module - Project 3
	COP4610 - Operating Systems 
		    Fall 2011
--------------------------------------- */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");

/* Function prototypes */
void init_vars(void);
void ferry_loop(void);
void begin_ferry(void);
void release_passengers(void);
void arrive_north(void);
void arrive_south(void);
void print_proc(void);
void travel(void);
void redeposit_south(void);
void redeposit_north(void);

/* Extern system call stub declarations */
extern long (*STUB_ferry_start)(void);
extern long (*STUB_ferry_request)(int passenger_type, char start_bank);
extern long (*STUB_ferry_stop)(void);

/* PROC declarations */
static struct proc_dir_entry *proc_entry;
static const char *PROC_NAME = "ferry";
int proc_read(char *page, char **pages, off_t offset, int page_size, int *eof, void *data);

/* Global Vars */
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
int stop_ferry_flag;
int queen_north_flag;
int queen_south_flag;
int ferry_active = 0;	//0 = not active, 1 = active
struct task_struct *ferry_thread;
struct mutex lock;

long my_ferry_start()
{
	//Handle if ferry gets a start request after already starting
	if(ferry_active == 1)
	{
		printk("%s: Ferry has already been started\n", __FUNCTION__);		
		return 1;
	}

	printk("%s: Ferry started!\n", __FUNCTION__);

	//Initialize mutex
	mutex_init(&lock);	
	
	//Initialize all variables
	init_vars();		

	//Initialize proc entry
	proc_entry = create_proc_entry(PROC_NAME, 0444, NULL);

	//begin ferry thread
	ferry_thread = kthread_run(begin_ferry, NULL, "ferry_thread");

	return 0;
}

void begin_ferry()
{
	//Continue looping ferry until stopped
	while(stop_ferry_flag != 1)
		ferry_loop();

	printk("%s: **THREAD STOPPING**\n", __FUNCTION__);
}


int proc_read(char *page, char **pages, off_t offset, int page_size, int *eof, void *data)
{
	//Writing to proc file
	int ret, pass_aboard;
	char * bank;
	if(curr_ferry.curr_bank == 0)
		bank = "North";
	else if(curr_ferry.curr_bank == 1)
		bank = "South";
	else if(curr_ferry.curr_bank == 2)
		bank = "In Transit";
	else
		bank = "Unknown";

	pass_aboard = curr_ferry.queen + curr_ferry.nobles + curr_ferry.peasants;
    
	ret = sprintf(page, 
                  "Current bank: %s\nPassengers aboard: %d\nCurrent passengers:\nQueen: %d\nNobles: %d\nPeasants: %d\nNorth bank:\nQueen: %d\nNobles: %d\nPeasants: %d\nSouth bank:\nQueen: %d\nNobles: %d\nPeasants: %d\nTotal ferried: %d\n", 
                  bank, pass_aboard, curr_ferry.queen, curr_ferry.nobles, curr_ferry.peasants, 
                  north_bank.queen, north_bank.nobles, north_bank.peasants, south_bank.queen, 
                  south_bank.nobles, south_bank.peasants, total_ferried);

    	return ret;
}

void release_passengers()
{
	curr_ferry.queen = 0;
	curr_ferry.nobles = 0;
	curr_ferry.peasants = 0;
	curr_ferry.compartments = 0;
	curr_ferry_passengers = 0;
}

void arrive_north()
{
	//lock
	mutex_lock_interruptible(&lock);
	printk("%s: **Locked**\n", __FUNCTION__);

	//arrive at north_bank
	curr_ferry.curr_bank = 0;
	printk("%s: Ferry arrived at north bank\n", __FUNCTION__);

	print_proc();

	release_passengers();

	printk("%s: Passengers released, unlocking and waiting\n", __FUNCTION__);
	//release lock
	mutex_unlock(&lock);
	//Wait 2 seconds for any extra passengers
	msleep(2000);
	//Lock again and begin loading passengers
	mutex_lock_interruptible(&lock);
	printk("%s: Waiting over, locked and loading passengers\n", __FUNCTION__);

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
	//releaseLock()
	mutex_unlock(&lock);
	printk("%s: **Unlocked**\n", __FUNCTION__);
}

void arrive_south()
{
	//lock
	mutex_lock_interruptible(&lock);
	printk("%s: **Locked**\n", __FUNCTION__);
	
	//arrive at south_bank
	printk("%s: Arrived at south bank\n", __FUNCTION__);
	curr_ferry.curr_bank = 1;

	print_proc();

	release_passengers();

	printk("%s: Passengers released, unlocking and waiting\n", __FUNCTION__);
	//release lock
	mutex_unlock(&lock);
	//Wait 2 seconds for any extra passengers
	msleep(2000);
	//Lock again and begin loading passengers
	mutex_lock_interruptible(&lock);
	printk("%s: Waiting over, locked and loading passengers\n", __FUNCTION__);

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
	//release lock
	mutex_unlock(&lock);
	printk("%s: **Unlocked**\n", __FUNCTION__);
}

void travel()
{
	printk("%s: Travelling: Watching for arriving queens\n", __FUNCTION__);
	//WATCHING FOR QUEEN ARRIVAL
	int n;
	for(n = 10; n > 0; n--)
	{
		msleep(200);
		if(queen_north_flag==1)
		{
			printk("%s: QUEEN NORTH FLAG; Going to north bank\n", __FUNCTION__);
			break;
			//shit hit the fan!
		}
		else if(queen_south_flag==1)
		{
			printk("%s: QUEEN SOUTH FLAG; Going to south bank\n", __FUNCTION__);
			break;
			//shit also hit the fan!
		}
	}
}

void redeposit_north()
{
	if(curr_ferry.queen == 1)
	{
		north_bank.queen++;
		curr_ferry.queen--;
	}
	if(curr_ferry.nobles > 0)
	{
		while(curr_ferry.nobles > 0)
		{
			north_bank.nobles++;
			curr_ferry.nobles--;
		}
	}
	if(curr_ferry.peasants > 0)
	{
		while(curr_ferry.peasants > 0)
		{
			north_bank.peasants++;
			curr_ferry.peasants--;
		}
	}
	printk("%s: North bank passengers redeposited\n", __FUNCTION__);
}

void redeposit_south()
{
	if(curr_ferry.queen == 1)
	{
		south_bank.queen++;
		curr_ferry.queen--;
	}
	if(curr_ferry.nobles > 0)
	{
		while(curr_ferry.nobles > 0)
		{
			south_bank.nobles++;
			curr_ferry.nobles--;
		}
	}
	if(curr_ferry.peasants > 0)
	{
		while(curr_ferry.peasants > 0)
		{
			south_bank.peasants++;
			curr_ferry.peasants--;
		}
	}
	printk("%s: South bank passengers redeposited\n", __FUNCTION__);
}

void ferry_loop()
{
	/*
	Start, check if the south flag is checked, if so
	we want to skip doing north and just go straight to south.
	Once we get to south we want to check if the flag was set
	and if so we want to redeposit passengers before we pick up again.

	The same goes for the second loop, we check to see if the north flag
	was checked, if so we want to skip the south and go straight back 
	to north and pick up the queen there. 

	If the flag is set and we return to a bank that we were travelling away
	from, we want to re-deposit the passengers currently on the ferry back
	onto the shore, then reload them in the correct order (queen first)
	*/

	if(queen_south_flag!=1)
	{
		if(queen_north_flag==1)
		{
			queen_north_flag=0;
			//redeposit passengers
			redeposit_north();
		}
		arrive_north();
		//Set bank to transit
		curr_ferry.curr_bank = 2;
		print_proc();
		travel();
		//We reset the south flag as to not deposit passengers that wanted
		//To go to south in the first place, then they get mixed up with north
		//Passengers and go back to where they came from
		queen_south_flag=0;
	}

	//if north has queen waiting skip south and go north
	if(queen_north_flag!=1)
	{
		//if south has queen we are already here so just reset flag
		if(queen_south_flag==1)
		{
			queen_south_flag=0;
			//reposit passengers
			redeposit_south();
		}
		arrive_south();
		//Set bank to transit
		curr_ferry.curr_bank = 2;
		print_proc();
		travel();
		queen_north_flag=0;
	}
}

void print_proc()
{
	//Write to proc file
	if(proc_entry != NULL)
                proc_entry->read_proc = proc_read;
}

void init_vars()
{
	//Initialize all variables
	north_bank.queen = 0;
	north_bank.nobles = 0;
	north_bank.peasants = 0;
	south_bank.queen = 0;
	south_bank.nobles = 0;
	south_bank.peasants = 0;
	curr_ferry.queen = 0;
	curr_ferry.nobles = 0;
	curr_ferry.peasants = 0;
	curr_ferry.curr_bank = 0;	//starts at north
	curr_ferry.compartments = 0;
	curr_ferry_passengers = 0;
	total_ferried = 0;
	stop_ferry_flag = 0;	//0 = keep going, 1 = stop
	queen_north_flag = 0;
	queen_south_flag = 0;
	ferry_active = 1;		//setting ferry to active
}

long my_ferry_request(int passenger_type, char start_bank)
{
    printk("%s: Ferry request %i, %c \n", __FUNCTION__, passenger_type, start_bank);
    
    //lock
    mutex_lock_interruptible(&lock);
    
	if(passenger_type == 0)
	{
		if(start_bank == 'n')
		{
			if(north_bank.queen == 1)
			{
				printk("%s: Too many north queens\n", __FUNCTION__);
				mutex_unlock(&lock);
				return 1;
			}
			else
			{
				north_bank.queen++;
				//We don't want to flag the boat to return to a queen
				//if the boat already has a queen on board
				//because then the bank would have two queens
				if(curr_ferry.queen == 0)
					queen_north_flag=1;
			}
		}
		else if(start_bank == 's')
		{
			if(south_bank.queen == 1)
			{
				printk("%s: Too many south queens\n", __FUNCTION__);
				mutex_unlock(&lock);
				return 1;
			}
			else
			{
				south_bank.queen++;
				if(curr_ferry.queen == 0)
					queen_south_flag=1;
			}	
		}
	}
    else if(passenger_type == 1)
    {
        if(start_bank == 'n')
            north_bank.nobles++;
        else if(start_bank == 's')
            south_bank.nobles++;
    }
    else if(passenger_type == 2)
    {
        if(start_bank == 'n')
            north_bank.peasants++;
        else if(start_bank == 's')
            south_bank.peasants++;
    }
    else
    {
        mutex_unlock(&lock);
        return 1;
    }
    //release lock
    mutex_unlock(&lock);
    return 0;
}

long my_ferry_stop(void)
{
	long ret = 0;
	printk("%s: Ferry stop \n", __FUNCTION__);
	if(stop_ferry_flag == 1)
		ret = 1;	//Already stopping
	else
		stop_ferry_flag = 1;	//ALL STOP!

	remove_proc_entry(PROC_NAME, 0);

	ferry_active = 0;
	return ret;
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
