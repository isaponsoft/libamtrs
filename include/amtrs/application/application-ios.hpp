/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__application__application_ios__hpp
#define	__libamtrs__application__application_ios__hpp
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "../os/ios/app_delegate.hpp"
#include "../filesystem/@"


@interface iosAppUIControoler : UIViewController
{
}
- (void)viewDidLoad;
- (BOOL)prefersStatusBarHidden;
@end


/*
@implementation AppDelegate
@synthesize window;

- (void)onUpdate;
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
- (BOOL)prefersStatusBarHidden;
- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillTerminate:(UIApplication *)application;
@end


*/
AMTRS_NAMESPACE_BEGIN

inline void application::foreground()
{
}

inline std::string application::cache_dir() const
{@autoreleasepool{
	NSArray*	paths	= NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString*	dir		= [paths objectAtIndex:0];
	const char*	cpath	= [dir fileSystemRepresentation];
	return	cpath;
}}


inline std::string application::files_dir(bool _external) const
{@autoreleasepool{
	NSArray*	paths	= NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
	NSString*	dir		= [paths objectAtIndex:0];
	const char*	cpath	= [dir fileSystemRepresentation];
	return	cpath;
}}


inline std::string application::documents_dir() const
{@autoreleasepool{
	NSArray*	paths	= NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString*	dir		= [paths objectAtIndex:0];
	const char*	cpath	= [dir fileSystemRepresentation];
	return	cpath;
}}


AMTRS_NAMESPACE_END
#endif
