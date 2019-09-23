/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__ios__app_delegate__hpp
#define	__libamtrs__ios__app_delegate__hpp

extern "C" {
#import <UIKit/UIKit.h>
}

#include "def.hpp"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
}
@property (nonatomic, retain) IBOutlet UIWindow*			mainWindow;
@property (nonatomic, retain) IBOutlet UIViewController*	mainController;
- (void)onUpdate;
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
- (BOOL)prefersStatusBarHidden;
- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillTerminate:(UIApplication *)application;
@end


AMTRS_OS_IOS_NAMESPACE_BEGIN
class	app_instance
{
public:
	static inline app_instance* get_instance()
	{
		static app_instance	s_thiz;
		return	&s_thiz;
	}

	AppDelegate* get_app_delegate() const noexcept
	{
		return	mAppDelegate;
	}

	void set_app_delegate(AppDelegate* _ad) noexcept
	{
		mAppDelegate	= _ad;
	}

	UIWindow* get_window() noexcept
	{
		return	get_app_delegate().mainWindow;
	}

	UIViewController* get_controller() noexcept
	{
		return	get_app_delegate().mainController;
	}

private:
	AppDelegate*	mAppDelegate	= nullptr;
};
AMTRS_OS_IOS_NAMESPACE_END
#endif
