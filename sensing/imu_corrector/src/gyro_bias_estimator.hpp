// Copyright 2023 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef GYRO_BIAS_ESTIMATOR_HPP_
#define GYRO_BIAS_ESTIMATOR_HPP_

#include "gyro_bias_estimation_module.hpp"
#include "tier4_autoware_utils/ros/transform_listener.hpp"

#include <diagnostic_updater/diagnostic_updater.hpp>
#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/vector3_stamped.hpp>
#include <sensor_msgs/msg/imu.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace imu_corrector
{
class GyroBiasEstimator : public rclcpp::Node
{
private:
  using Imu = sensor_msgs::msg::Imu;
  using PoseWithCovarianceStamped = geometry_msgs::msg::PoseWithCovarianceStamped;
  using Vector3Stamped = geometry_msgs::msg::Vector3Stamped;
  using Vector3 = geometry_msgs::msg::Vector3;

public:
  GyroBiasEstimator();

private:
  void update_diagnostics(diagnostic_updater::DiagnosticStatusWrapper & stat);
  void callback_imu(const Imu::ConstSharedPtr imu_msg_ptr);
  void callback_pose(const PoseWithCovarianceStamped::ConstSharedPtr pose_msg_ptr);
  void timer_callback();

  static geometry_msgs::msg::Vector3 transform_vector3(
    const geometry_msgs::msg::Vector3 & vec,
    const geometry_msgs::msg::TransformStamped & transform);

  const std::string output_frame_ = "base_link";

  rclcpp::Subscription<Imu>::SharedPtr imu_sub_;
  rclcpp::Subscription<PoseWithCovarianceStamped>::SharedPtr pose_sub_;
  rclcpp::Publisher<Vector3Stamped>::SharedPtr gyro_bias_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<GyroBiasEstimationModule> gyro_bias_estimation_module_;

  const double gyro_bias_threshold_;
  const double angular_velocity_offset_x_;
  const double angular_velocity_offset_y_;
  const double angular_velocity_offset_z_;
  const double timer_callback_interval_sec_;
  const double straight_motion_ang_vel_upper_limit_;

  diagnostic_updater::Updater updater_;

  std::optional<Vector3> gyro_bias_;

  std::shared_ptr<tier4_autoware_utils::TransformListener> transform_listener_;

  std::string imu_frame_;

  std::vector<geometry_msgs::msg::Vector3Stamped> gyro_all_;
  std::vector<geometry_msgs::msg::PoseStamped> pose_buf_;
};
}  // namespace imu_corrector

#endif  // GYRO_BIAS_ESTIMATOR_HPP_
