/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__application__application_ios__hpp
#define	__libamtrs__application__application_ios__hpp
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "../../os/ios/app_delegate.hpp"
#include "../../filesystem.hpp"


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

inline filesystem::path application::cache_dir() const
{@autoreleasepool{
	NSArray*	paths	= NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString*	dir		= [paths objectAtIndex:0];
	const char*	cpath	= [dir fileSystemRepresentation];
	return	cpath;
}}


inline filesystem::path application::files_dir(bool _external) const
{@autoreleasepool{
	NSArray*	paths	= NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
	NSString*	dir		= [paths objectAtIndex:0];
	const char*	cpath	= [dir fileSystemRepresentation];
	return	cpath;
}}


inline filesystem::path application::documents_dir() const
{@autoreleasepool{
	NSArray*	paths	= NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString*	dir		= [paths objectAtIndex:0];
	const char*	cpath	= [dir fileSystemRepresentation];
	return	cpath;
}}


AMTRS_NAMESPACE_END
#endif
