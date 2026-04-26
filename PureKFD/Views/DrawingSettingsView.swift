import SwiftUI

struct DrawingSettingsView: View {
    @State private var masterEnabled = true
    @State private var blockBots = false
    @State private var drawPlayer = true
    @State private var drawBox = true
    @State private var drawSkeleton = true
    @State private var drawRay = true
    @State private var showInfo = true
    @State private var showWeapon = true
    @State private var backHint = true
    @State private var radarSectionEnabled = true
    @State private var radarEnabled = true

    @State private var radarX: Double = 134
    @State private var radarY: Double = 93

    private let onColor = Color(red: 1.0, green: 0.16, blue: 0.32)
    private let bgColor = Color(red: 0.08, green: 0.09, blue: 0.11)
    private let cardColor = Color(red: 0.11, green: 0.12, blue: 0.15)

    var body: some View {
        ZStack {
            bgColor.ignoresSafeArea()

            VStack(spacing: 10) {
                header

                ScrollView(showsIndicators: false) {
                    VStack(spacing: 10) {
                        settingsCard
                        radarCard
                    }
                    .padding(.horizontal, 14)
                    .padding(.bottom, 14)
                }
            }
        }
        .preferredColorScheme(.dark)
    }

    private var header: some View {
        HStack {
            Text("绘制")
                .font(.system(size: 24, weight: .semibold))
                .foregroundStyle(.white)

            Spacer()

            Button(action: {}) {
                Text("绘制")
                    .font(.system(size: 14, weight: .bold))
                    .foregroundStyle(.white)
                    .padding(.horizontal, 14)
                    .padding(.vertical, 8)
                    .background(onColor)
                    .clipShape(Capsule())
            }
        }
        .padding(.horizontal, 16)
        .padding(.top, 8)
        .padding(.bottom, 4)
    }

    private var settingsCard: some View {
        VStack(spacing: 0) {
            SettingToggleRow(title: "总开关", isOn: $masterEnabled, onColor: onColor)
            SettingToggleRow(title: "屏蔽人机", isOn: $blockBots, onColor: onColor)
            SettingToggleRow(title: "人物绘制", isOn: $drawPlayer, onColor: onColor)
            SettingToggleRow(title: "方框绘制", isOn: $drawBox, onColor: onColor)
            SettingToggleRow(title: "骨骼绘制", isOn: $drawSkeleton, onColor: onColor)
            SettingToggleRow(title: "射线绘制", isOn: $drawRay, onColor: onColor)
            SettingToggleRow(title: "信息显示", isOn: $showInfo, onColor: onColor)
            SettingToggleRow(title: "手持武器", isOn: $showWeapon, onColor: onColor)
            SettingToggleRow(title: "背?提示", isOn: $backHint, onColor: onColor)
            SettingToggleRow(title: "雷达设置", isOn: $radarSectionEnabled, onColor: onColor)
        }
        .padding(.horizontal, 12)
        .padding(.vertical, 4)
        .background(cardColor)
        .clipShape(RoundedRectangle(cornerRadius: 12, style: .continuous))
    }

    private var radarCard: some View {
        VStack(alignment: .leading, spacing: 12) {
            SettingToggleRow(title: "雷达开关", isOn: $radarEnabled, onColor: onColor, noDivider: true)

            VStack(spacing: 10) {
                SliderRow(title: "X 位置", value: $radarX, range: 0...300, tint: onColor)
                SliderRow(title: "Y 位置", value: $radarY, range: 0...300, tint: onColor)
            }
        }
        .padding(.horizontal, 12)
        .padding(.vertical, 12)
        .background(cardColor)
        .clipShape(RoundedRectangle(cornerRadius: 12, style: .continuous))
    }
}

private struct SettingToggleRow: View {
    let title: String
    @Binding var isOn: Bool
    let onColor: Color
    var noDivider: Bool = false

    var body: some View {
        VStack(spacing: 0) {
            HStack {
                Text(title)
                    .font(.system(size: 15, weight: .medium))
                    .foregroundStyle(.white)

                Spacer()

                Toggle("", isOn: $isOn)
                    .labelsHidden()
                    .tint(onColor)
                    .scaleEffect(0.95)
            }
            .frame(height: 42)

            if !noDivider {
                Divider().overlay(Color.white.opacity(0.06))
            }
        }
    }
}

private struct SliderRow: View {
    let title: String
    @Binding var value: Double
    let range: ClosedRange<Double>
    let tint: Color

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            HStack {
                Text(title)
                    .font(.system(size: 14, weight: .medium))
                    .foregroundStyle(.white.opacity(0.9))

                Spacer()

                Text("\(Int(value))px")
                    .font(.system(size: 14, weight: .semibold))
                    .foregroundStyle(tint)
            }

            Slider(value: $value, in: range)
                .tint(tint)
        }
    }
}

#Preview {
    DrawingSettingsView()
}
