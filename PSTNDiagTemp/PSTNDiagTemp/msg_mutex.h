#pragma once
#include <mutex>

/*====================================================================
* AUTHOR: Hagai Vinik	.	
* DATE: 16/05/19		.	
* DESCRIPTION: This file declares all mutex that are used by this
*				software and needed for using resources properly	. 
* SOURCE: copyright (c) 2018 Hagai Vinik , all rights reserved.
======================================================================*/


static std::mutex mtx_message;
static std::mutex Comm_mtx_msg;
static std::mutex Comm_mtx_msg_to_server;
static std::mutex mtx_file;