/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <amtrs/inc/application/application.hpp>


struct	ios_application_status
{
	bool				active		= false;		//!< app_delegateによってアクティブになったかどうかを判定するフラグ

	bool				created		= false;
	bool				stopped		= false;		//!< stop イベント発行済みの時にtrue
	bool				resumed		= false;		//!< resume イベント発行済みの時にtrue
	NSTimer*			timer		= nullptr;
}		g_appliStatus;



@implementation AppDelegate
@synthesize window;

- (void)onUpdate
{
	using namespace amtrs;
	if (g_appliStatus.active)
	{
		// アプリケーションの on_created() を呼び出す。
		if (!g_appliStatus.created)
		{
			g_appliStatus.created	= true;
			application::get_application()->on_created(0, nullptr);
		}

		if (!g_appliStatus.resumed)
		{
			AMTRS_DEBUG_LOG("RESUME");
			g_appliStatus.resumed	= true;
			if (g_appliStatus.stopped)
			{
				g_appliStatus.stopped	= false;
				application::get_application()->on_event(application::system_event::restart);
			}
			application::get_application()->on_event(application::system_event::start);
			application::get_application()->on_event(application::system_event::resume);
		}

		application::get_application()->on_update();
		if (application::get_application()->is_quit())
		{
			AMTRS_DEBUG_LOG("QUIT");
			[g_appliStatus.timer invalidate];
			g_appliStatus.timer	= nullptr;
		}
	}
	else
	{
		if (!g_appliStatus.stopped)
		{
			AMTRS_DEBUG_LOG("STOP");
			g_appliStatus.resumed	= false;
			application::get_application()->on_event(application::system_event::pause);
			g_appliStatus.stopped	= true;
			application::get_application()->on_event(application::system_event::stop);
		}
	}
}


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	using namespace amtrs;
#ifdef	DEBUG
	{
		CGRect	ss		= [[UIScreen mainScreen] bounds];
		CGFloat scale	= [UIScreen mainScreen].scale;
		auto	width	= ss.size.width  * scale;
		auto	height	= ss.size.height * scale;
		AMTRS_DEBUG_LOG("iOS screen size %dx%d, base size=%dx%d, scale=%2.1f", (int)width, (int)height, (int)ss.size.width, (int)ss.size.height, (float)scale);
	}
#endif

	os::ios::app_instance::get_instance()->set_app_delegate(self);

	// おおもとのウィンドウを作っておく
	self.mainWindow						= [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.mainWindow.backgroundColor		= [UIColor whiteColor];
	
	self.mainController					= [[iosAppUIControoler alloc] init];
	self.mainWindow.rootViewController	= self.mainController;
    [self.mainWindow makeKeyAndVisible];

	g_appliStatus.active	= true;
	g_appliStatus.timer	= [NSTimer
						scheduledTimerWithTimeInterval:	1.0f/60.0f
						target:							self selector:@selector(onUpdate) userInfo:nil repeats:YES
					];


	return YES;
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	AMTRS_DEBUG_LOG("applicationWillResignActive");
	g_appliStatus.active	= false;
}


- (void)applicationDidEnterBackground:(UIApplication *)application
{
	AMTRS_DEBUG_LOG("applicationDidEnterBackground");
}


- (void)applicationWillEnterForeground:(UIApplication *)application
{
	AMTRS_DEBUG_LOG("applicationWillEnterForeground");
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
	AMTRS_DEBUG_LOG("applicationDidBecomeActive");
	g_appliStatus.active	= true;
}


- (void)applicationWillTerminate:(UIApplication *)application
{
	AMTRS_DEBUG_LOG("applicationWillTerminate");
}

@end


@implementation iosAppUIControoler

- (void)viewDidLoad
{
	[super viewDidLoad];
	
	// ステータスバーの表示/非表示メソッド呼び出し
	if ([self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)])
	{
		// iOS 7以降
		[self prefersStatusBarHidden];
		[self performSelector:@selector(setNeedsStatusBarAppearanceUpdate)];
	}
#if	0
	else
	{
		// iOS 7未満
		[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationSlide];
	}
#endif
}

- (BOOL)prefersStatusBarHidden
{
	return	YES;	// ステータスバーを非表示にすべきかを返す
}
@end
