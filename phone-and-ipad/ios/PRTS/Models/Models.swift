import Foundation

// MARK: - User
struct User: Codable, Identifiable {
    let id: String
    var username: String
    var email: String
    var avatarURL: String?
    var role: Role

    enum Role: String, Codable {
        case admin
        case developer
        case viewer
    }
}

// MARK: - Pipeline
struct Pipeline: Codable, Identifiable {
    let id: String
    var name: String
    var status: Status
    var branch: String
    var duration: String
    var commit: String?
    var triggeredBy: String?
    var stages: [Stage]?

    enum Status: String, Codable {
        case success
        case running
        case failed
        case pending
        case cancelled

        var color: SwiftUI.Color {
            switch self {
            case .success: return .green
            case .running: return .blue
            case .failed: return .red
            case .pending: return .gray
            case .cancelled: return .orange
            }
        }
    }

    struct Stage: Codable, Identifiable {
        let id: String
        var name: String
        var status: Status
        var duration: String
        var jobs: [Job]
    }

    struct Job: Codable, Identifiable {
        let id: String
        var name: String
        var status: Status
        var duration: String
    }
}

// MARK: - Alert
struct AlertItem: Codable, Identifiable {
    let id: String
    var title: String
    var message: String
    var severity: Severity
    var source: String
    var timestamp: Date
    var acknowledged: Bool

    var timeAgo: String {
        let formatter = RelativeDateTimeFormatter()
        formatter.unitsStyle = .abbreviated
        return formatter.localizedString(for: timestamp, relativeTo: Date())
    }

    enum Severity: String, Codable {
        case critical
        case warning
        case info

        var color: SwiftUI.Color {
            switch self {
            case .critical: return .red
            case .warning: return .orange
            case .info: return .blue
            }
        }

        var icon: String {
            switch self {
            case .critical: return "exclamationmark.triangle.fill"
            case .warning: return "exclamationmark.circle.fill"
            case .info: return "info.circle.fill"
            }
        }
    }
}

// MARK: - Project
struct Project: Codable, Identifiable {
    let id: String
    var name: String
    var description: String
    var emoji: String
    var pipelinesCount: Int
    var membersCount: Int
    var lastActivity: String
}

// MARK: - Metrics
struct Metrics: Codable {
    var cpuUsage: Double
    var memoryUsage: Double
    var diskUsage: Double
    var networkIn: Double
    var networkOut: Double
    var requestsPerSecond: Double
    var errorRate: Double
}

// MARK: - API Response
struct APIResponse<T: Codable>: Codable {
    let success: Bool
    let data: T?
    let error: APIError?
}

struct APIError: Codable {
    let code: String
    let message: String
}
