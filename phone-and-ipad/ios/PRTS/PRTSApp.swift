import SwiftUI

@main
struct PRTSApp: App {
    @StateObject private var appState = AppState()
    @UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(appState)
                .preferredColorScheme(.dark)
        }
    }
}

// MARK: - App Delegate for Push Notifications
class AppDelegate: NSObject, UIApplicationDelegate, UNUserNotificationCenterDelegate {
    func application(
        _ application: UIApplication,
        didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?
    ) -> Bool {
        // 配置推送通知
        UNUserNotificationCenter.current().delegate = self
        registerForPushNotifications()
        return true
    }

    func registerForPushNotifications() {
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { granted, error in
            if granted {
                DispatchQueue.main.async {
                    UIApplication.shared.registerForRemoteNotifications()
                }
            }
        }
    }

    func application(
        _ application: UIApplication,
        didRegisterForRemoteNotificationsWithDeviceToken deviceToken: Data
    ) {
        let token = deviceToken.map { String(format: "%02.2hhx", $0) }.joined()
        print("APNs Token: \(token)")
        // TODO: 发送 token 到服务器
    }

    func application(
        _ application: UIApplication,
        didFailToRegisterForRemoteNotificationsWithError error: Error
    ) {
        print("Failed to register for push notifications: \(error)")
    }

    // 前台收到通知
    func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        willPresent notification: UNNotification,
        withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void
    ) {
        completionHandler([.banner, .sound, .badge])
    }

    // 用户点击通知
    func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        didReceive response: UNNotificationResponse,
        withCompletionHandler completionHandler: @escaping () -> Void
    ) {
        let userInfo = response.notification.request.content.userInfo
        handleNotification(userInfo)
        completionHandler()
    }

    private func handleNotification(_ userInfo: [AnyHashable: Any]) {
        // 处理通知，如跳转到对应页面
        if let type = userInfo["type"] as? String {
            switch type {
            case "pipeline":
                NotificationCenter.default.post(name: .navigateToPipeline, object: userInfo)
            case "alert":
                NotificationCenter.default.post(name: .navigateToAlerts, object: userInfo)
            default:
                break
            }
        }
    }
}

// MARK: - Notification Names
extension Notification.Name {
    static let navigateToPipeline = Notification.Name("navigateToPipeline")
    static let navigateToAlerts = Notification.Name("navigateToAlerts")
}

// MARK: - App State
class AppState: ObservableObject {
    @Published var isLoggedIn = false
    @Published var user: User?
    @Published var selectedTab: Tab = .dashboard

    enum Tab: String, CaseIterable {
        case dashboard = "监控"
        case pipelines = "流水线"
        case alerts = "告警"
        case projects = "项目"

        var icon: String {
            switch self {
            case .dashboard: return "chart.bar.fill"
            case .pipelines: return "arrow.triangle.branch"
            case .alerts: return "bell.fill"
            case .projects: return "folder.fill"
            }
        }
    }
}
