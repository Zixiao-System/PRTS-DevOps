import ActivityKit
import WidgetKit
import SwiftUI

// MARK: - Live Activity Attributes
struct PipelineActivityAttributes: ActivityAttributes {
    public struct ContentState: Codable, Hashable {
        var status: String
        var currentStage: String
        var progress: Double
        var elapsedTime: String
    }

    var pipelineName: String
    var branch: String
}

// MARK: - Live Activity Widget
struct PipelineActivityWidget: Widget {
    var body: some WidgetConfiguration {
        ActivityConfiguration(for: PipelineActivityAttributes.self) { context in
            // Lock Screen / Banner UI
            LockScreenView(context: context)
        } dynamicIsland: { context in
            DynamicIsland {
                // Expanded UI
                DynamicIslandExpandedRegion(.leading) {
                    HStack(spacing: 8) {
                        Image(systemName: statusIcon(for: context.state.status))
                            .foregroundStyle(statusColor(for: context.state.status))
                        Text(context.attributes.pipelineName)
                            .font(.headline)
                    }
                }

                DynamicIslandExpandedRegion(.trailing) {
                    Text(context.state.elapsedTime)
                        .font(.caption)
                        .foregroundStyle(.secondary)
                }

                DynamicIslandExpandedRegion(.bottom) {
                    VStack(spacing: 8) {
                        HStack {
                            Text(context.state.currentStage)
                                .font(.subheadline)
                            Spacer()
                            Text("\(Int(context.state.progress * 100))%")
                                .font(.caption)
                        }

                        ProgressView(value: context.state.progress)
                            .tint(statusColor(for: context.state.status))
                    }
                    .padding(.horizontal)
                }
            } compactLeading: {
                // Compact Leading
                Image(systemName: statusIcon(for: context.state.status))
                    .foregroundStyle(statusColor(for: context.state.status))
            } compactTrailing: {
                // Compact Trailing
                Text("\(Int(context.state.progress * 100))%")
                    .font(.caption2)
            } minimal: {
                // Minimal
                Image(systemName: statusIcon(for: context.state.status))
                    .foregroundStyle(statusColor(for: context.state.status))
            }
        }
    }

    private func statusIcon(for status: String) -> String {
        switch status {
        case "running": return "arrow.triangle.2.circlepath"
        case "success": return "checkmark.circle.fill"
        case "failed": return "xmark.circle.fill"
        default: return "circle"
        }
    }

    private func statusColor(for status: String) -> Color {
        switch status {
        case "running": return .blue
        case "success": return .green
        case "failed": return .red
        default: return .gray
        }
    }
}

// MARK: - Lock Screen View
struct LockScreenView: View {
    let context: ActivityViewContext<PipelineActivityAttributes>

    var body: some View {
        VStack(spacing: 12) {
            HStack {
                VStack(alignment: .leading, spacing: 4) {
                    Text(context.attributes.pipelineName)
                        .font(.headline)
                    Text(context.attributes.branch)
                        .font(.caption)
                        .foregroundStyle(.secondary)
                }

                Spacer()

                Text(context.state.elapsedTime)
                    .font(.caption)
                    .padding(.horizontal, 8)
                    .padding(.vertical, 4)
                    .background(.ultraThinMaterial)
                    .cornerRadius(4)
            }

            VStack(spacing: 4) {
                HStack {
                    Text(context.state.currentStage)
                        .font(.subheadline)
                    Spacer()
                    Text(context.state.status)
                        .font(.caption)
                        .foregroundStyle(statusColor)
                }

                ProgressView(value: context.state.progress)
                    .tint(statusColor)
            }
        }
        .padding()
        .background(.ultraThinMaterial)
    }

    private var statusColor: Color {
        switch context.state.status {
        case "running": return .blue
        case "success": return .green
        case "failed": return .red
        default: return .gray
        }
    }
}

// MARK: - Activity Manager
class PipelineActivityManager {
    static let shared = PipelineActivityManager()
    private var currentActivity: Activity<PipelineActivityAttributes>?

    func startActivity(pipelineName: String, branch: String) {
        guard ActivityAuthorizationInfo().areActivitiesEnabled else { return }

        let attributes = PipelineActivityAttributes(
            pipelineName: pipelineName,
            branch: branch
        )

        let initialState = PipelineActivityAttributes.ContentState(
            status: "running",
            currentStage: "Build",
            progress: 0.0,
            elapsedTime: "0s"
        )

        do {
            currentActivity = try Activity.request(
                attributes: attributes,
                content: .init(state: initialState, staleDate: nil),
                pushType: .token
            )
        } catch {
            print("Error starting Live Activity: \(error)")
        }
    }

    func updateActivity(status: String, stage: String, progress: Double, elapsed: String) {
        guard let activity = currentActivity else { return }

        let state = PipelineActivityAttributes.ContentState(
            status: status,
            currentStage: stage,
            progress: progress,
            elapsedTime: elapsed
        )

        Task {
            await activity.update(
                ActivityContent(state: state, staleDate: nil)
            )
        }
    }

    func endActivity(status: String) {
        guard let activity = currentActivity else { return }

        let finalState = PipelineActivityAttributes.ContentState(
            status: status,
            currentStage: status == "success" ? "完成" : "失败",
            progress: 1.0,
            elapsedTime: ""
        )

        Task {
            await activity.end(
                ActivityContent(state: finalState, staleDate: nil),
                dismissalPolicy: .after(.now + 300) // 5分钟后消失
            )
            currentActivity = nil
        }
    }
}
