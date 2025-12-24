import SwiftUI
import Charts

struct DashboardView: View {
    @StateObject private var viewModel = DashboardViewModel()

    var body: some View {
        NavigationStack {
            ScrollView {
                VStack(spacing: 20) {
                    // 顶部指标卡片
                    LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible())], spacing: 16) {
                        MetricCard(
                            title: "CPU 使用率",
                            value: viewModel.cpuUsage,
                            unit: "%",
                            trend: .up,
                            trendValue: "+5%"
                        )

                        MetricCard(
                            title: "内存使用",
                            value: viewModel.memoryUsage,
                            unit: "GB",
                            trend: .down,
                            trendValue: "-0.3"
                        )

                        MetricCard(
                            title: "构建成功率",
                            value: viewModel.buildSuccessRate,
                            unit: "%",
                            trend: .up,
                            trendValue: "+2%"
                        )

                        MetricCard(
                            title: "活跃服务",
                            value: Double(viewModel.activeServices),
                            unit: "",
                            trend: .stable,
                            trendValue: ""
                        )
                    }
                    .padding(.horizontal)

                    // CPU 图表
                    VStack(alignment: .leading, spacing: 12) {
                        Text("CPU 使用率趋势")
                            .font(.headline)
                            .padding(.horizontal)

                        Chart(viewModel.cpuHistory) { data in
                            LineMark(
                                x: .value("时间", data.timestamp),
                                y: .value("使用率", data.value)
                            )
                            .foregroundStyle(.blue)

                            AreaMark(
                                x: .value("时间", data.timestamp),
                                y: .value("使用率", data.value)
                            )
                            .foregroundStyle(.blue.opacity(0.1))
                        }
                        .frame(height: 200)
                        .padding(.horizontal)
                    }
                    .padding(.vertical)
                    .background(Color(.systemGray6))
                    .cornerRadius(16)
                    .padding(.horizontal)

                    // 最近流水线
                    VStack(alignment: .leading, spacing: 12) {
                        HStack {
                            Text("最近流水线")
                                .font(.headline)
                            Spacer()
                            NavigationLink("查看全部") {
                                PipelineListView()
                            }
                            .font(.subheadline)
                        }
                        .padding(.horizontal)

                        ForEach(viewModel.recentPipelines) { pipeline in
                            PipelineRow(pipeline: pipeline)
                        }
                    }
                    .padding(.vertical)
                    .background(Color(.systemGray6))
                    .cornerRadius(16)
                    .padding(.horizontal)
                }
                .padding(.vertical)
            }
            .navigationTitle("监控看板")
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    Button {
                        viewModel.refresh()
                    } label: {
                        Image(systemName: "arrow.clockwise")
                    }
                }
            }
            .refreshable {
                await viewModel.refreshAsync()
            }
        }
    }
}

// MARK: - Metric Card
struct MetricCard: View {
    let title: String
    let value: Double
    let unit: String
    let trend: Trend
    let trendValue: String

    enum Trend {
        case up, down, stable

        var color: Color {
            switch self {
            case .up: return .green
            case .down: return .red
            case .stable: return .gray
            }
        }

        var icon: String {
            switch self {
            case .up: return "arrow.up.right"
            case .down: return "arrow.down.right"
            case .stable: return "minus"
            }
        }
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text(title)
                .font(.caption)
                .foregroundStyle(.secondary)

            HStack(alignment: .lastTextBaseline, spacing: 2) {
                Text(String(format: "%.1f", value))
                    .font(.title)
                    .fontWeight(.bold)
                Text(unit)
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }

            if !trendValue.isEmpty {
                HStack(spacing: 4) {
                    Image(systemName: trend.icon)
                        .font(.caption2)
                    Text(trendValue)
                        .font(.caption)
                }
                .foregroundStyle(trend.color)
            }
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color(.systemGray6))
        .cornerRadius(12)
    }
}

// MARK: - ViewModel
class DashboardViewModel: ObservableObject {
    @Published var cpuUsage: Double = 42.0
    @Published var memoryUsage: Double = 6.2
    @Published var buildSuccessRate: Double = 94.0
    @Published var activeServices: Int = 12
    @Published var cpuHistory: [ChartData] = []
    @Published var recentPipelines: [Pipeline] = []

    init() {
        loadMockData()
    }

    func refresh() {
        loadMockData()
    }

    func refreshAsync() async {
        // 模拟网络请求
        try? await Task.sleep(nanoseconds: 1_000_000_000)
        await MainActor.run {
            loadMockData()
        }
    }

    private func loadMockData() {
        // 生成 CPU 历史数据
        let now = Date()
        cpuHistory = (0..<24).map { i in
            ChartData(
                timestamp: now.addingTimeInterval(-Double(i) * 3600),
                value: Double.random(in: 30...60)
            )
        }.reversed()

        // 模拟流水线数据
        recentPipelines = [
            Pipeline(id: "1", name: "frontend-deploy", status: .success, branch: "main", duration: "3m 42s"),
            Pipeline(id: "2", name: "backend-build", status: .running, branch: "develop", duration: "1m 20s"),
            Pipeline(id: "3", name: "integration-test", status: .failed, branch: "feature/auth", duration: "5m 10s")
        ]
    }
}

struct ChartData: Identifiable {
    let id = UUID()
    let timestamp: Date
    let value: Double
}

#Preview {
    DashboardView()
        .preferredColorScheme(.dark)
}
