import SwiftUI

struct ContentView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        TabView(selection: $appState.selectedTab) {
            DashboardView()
                .tabItem {
                    Label(AppState.Tab.dashboard.rawValue, systemImage: AppState.Tab.dashboard.icon)
                }
                .tag(AppState.Tab.dashboard)

            PipelineListView()
                .tabItem {
                    Label(AppState.Tab.pipelines.rawValue, systemImage: AppState.Tab.pipelines.icon)
                }
                .tag(AppState.Tab.pipelines)

            AlertsView()
                .tabItem {
                    Label(AppState.Tab.alerts.rawValue, systemImage: AppState.Tab.alerts.icon)
                }
                .tag(AppState.Tab.alerts)

            ProjectsView()
                .tabItem {
                    Label(AppState.Tab.projects.rawValue, systemImage: AppState.Tab.projects.icon)
                }
                .tag(AppState.Tab.projects)
        }
        .tint(.blue)
    }
}

#Preview {
    ContentView()
        .environmentObject(AppState())
        .preferredColorScheme(.dark)
}
