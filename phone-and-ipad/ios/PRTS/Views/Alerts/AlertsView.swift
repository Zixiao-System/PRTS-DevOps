import SwiftUI

struct AlertsView: View {
    @StateObject private var viewModel = AlertsViewModel()

    var body: some View {
        NavigationStack {
            List {
                ForEach(viewModel.alerts) { alert in
                    AlertRow(alert: alert) {
                        viewModel.acknowledge(alert)
                    }
                }
                .onDelete { indexSet in
                    viewModel.deleteAlerts(at: indexSet)
                }
            }
            .listStyle(.plain)
            .navigationTitle("告警")
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    Menu {
                        Button("全部已读") {
                            viewModel.acknowledgeAll()
                        }
                        Button("清除已解决", role: .destructive) {
                            viewModel.clearResolved()
                        }
                    } label: {
                        Image(systemName: "ellipsis.circle")
                    }
                }
            }
            .refreshable {
                await viewModel.refresh()
            }
            .overlay {
                if viewModel.alerts.isEmpty {
                    ContentUnavailableView(
                        "没有告警",
                        systemImage: "checkmark.seal.fill",
                        description: Text("所有系统运行正常")
                    )
                }
            }
        }
    }
}

// MARK: - Alert Row
struct AlertRow: View {
    let alert: AlertItem
    let onAcknowledge: () -> Void

    var body: some View {
        HStack(spacing: 12) {
            // 严重性图标
            Image(systemName: alert.severity.icon)
                .font(.title2)
                .foregroundStyle(alert.severity.color)
                .frame(width: 40)

            VStack(alignment: .leading, spacing: 4) {
                HStack {
                    Text(alert.title)
                        .font(.headline)

                    if !alert.acknowledged {
                        Circle()
                            .fill(.blue)
                            .frame(width: 8, height: 8)
                    }
                }

                Text(alert.message)
                    .font(.subheadline)
                    .foregroundStyle(.secondary)
                    .lineLimit(2)

                HStack {
                    Text(alert.source)
                        .font(.caption)
                        .padding(.horizontal, 6)
                        .padding(.vertical, 2)
                        .background(Color(.systemGray5))
                        .cornerRadius(4)

                    Text(alert.timeAgo)
                        .font(.caption)
                        .foregroundStyle(.secondary)
                }
            }

            Spacer()

            if !alert.acknowledged {
                Button {
                    onAcknowledge()
                } label: {
                    Image(systemName: "checkmark")
                        .padding(8)
                        .background(Color(.systemGray5))
                        .clipShape(Circle())
                }
                .buttonStyle(.plain)
            }
        }
        .padding(.vertical, 4)
    }
}

// MARK: - ViewModel
class AlertsViewModel: ObservableObject {
    @Published var alerts: [AlertItem] = []

    init() {
        loadAlerts()
    }

    func refresh() async {
        try? await Task.sleep(nanoseconds: 1_000_000_000)
        await MainActor.run {
            loadAlerts()
        }
    }

    func acknowledge(_ alert: AlertItem) {
        if let index = alerts.firstIndex(where: { $0.id == alert.id }) {
            alerts[index].acknowledged = true
        }
    }

    func acknowledgeAll() {
        for index in alerts.indices {
            alerts[index].acknowledged = true
        }
    }

    func clearResolved() {
        alerts.removeAll { $0.acknowledged }
    }

    func deleteAlerts(at offsets: IndexSet) {
        alerts.remove(atOffsets: offsets)
    }

    private func loadAlerts() {
        alerts = [
            AlertItem(
                id: "1",
                title: "CPU 使用率过高",
                message: "生产服务器 prod-01 CPU 使用率超过 90%",
                severity: .critical,
                source: "prod-01",
                timestamp: Date().addingTimeInterval(-300),
                acknowledged: false
            ),
            AlertItem(
                id: "2",
                title: "构建失败",
                message: "流水线 backend-build 在 Test 阶段失败",
                severity: .warning,
                source: "CI/CD",
                timestamp: Date().addingTimeInterval(-1800),
                acknowledged: false
            ),
            AlertItem(
                id: "3",
                title: "磁盘空间不足",
                message: "数据库服务器 db-01 磁盘使用率达到 85%",
                severity: .warning,
                source: "db-01",
                timestamp: Date().addingTimeInterval(-3600),
                acknowledged: true
            ),
            AlertItem(
                id: "4",
                title: "服务恢复",
                message: "API 网关服务已恢复正常",
                severity: .info,
                source: "gateway",
                timestamp: Date().addingTimeInterval(-7200),
                acknowledged: true
            )
        ]
    }
}

#Preview {
    AlertsView()
        .preferredColorScheme(.dark)
}
