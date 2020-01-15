/*
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef USB_H
#define USB_H

#include "Device.h"
#include "ResourceManager.h"
#include "QalAudioRoute.h"

#include <vector>

#define USB_BUFF_SIZE           2048
#define CHANNEL_NUMBER_STR      "Channels: "
#define PLAYBACK_PROFILE_STR    "Playback:"
#define CAPTURE_PROFILE_STR     "Capture:"
#define DATA_PACKET_INTERVAL_STR "Data packet interval:"
#define USB_SIDETONE_GAIN_STR   "usb_sidetone_gain"
// Supported sample rates for USB
#define USBID_SIZE                16
/* support positional and index masks to 8ch */
#define MAX_SUPPORTED_CHANNEL_MASKS 8
#define MAX_HIFI_CHANNEL_COUNT 8
#define MIN_CHANNEL_COUNT 1
#define DEFAULT_CHANNEL_COUNT 2
#define  MAX_SAMPLE_RATE_SIZE 14
#define DEFAULT_SERVICE_INTERVAL_US    0

typedef enum usb_usecase_type{
    USB_CAPTURE = 0,
    USB_PLAYBACK,
} usb_usecase_type_t;

// one card supports multiple devices
class USBDeviceConfig {
protected:
    unsigned int bit_width_;
    unsigned int channels_;
    unsigned int rate_size_;
    unsigned int rates_[MAX_SAMPLE_RATE_SIZE];
    unsigned long service_interval_us_;
    usb_usecase_type_t type_;
    unsigned int supported_sample_rates_mask_[2];
public:
    void setBitWidth(unsigned int bit_width);
    unsigned int getBitWidth();
    void setChannels(unsigned int channels);
    unsigned int getChannels();
    void setType(usb_usecase_type_t type);
    unsigned int getType();
    void setInterval(unsigned long interval);
    unsigned long getInterval();
    unsigned int getDefaultRate();
    int getSampleRates(int type, char *rates_str);
    int getBestRate(int requested_rate, unsigned int *best_rate);
    int getBestChInfo(struct qal_channel_info *requested_ch_info,
                        struct qal_channel_info **best);
    int getServiceInterval(const char *interval_str_start);
    static const unsigned int supported_sample_rates_[MAX_SAMPLE_RATE_SIZE];
};

class USBCardConfig {
protected:
    plugin_device_address_t address_;
    int endian_;
    std::vector <std::shared_ptr<USBDeviceConfig>> usb_device_config_list_;
public:
    USBCardConfig(plugin_device_address_t address);
    bool isConfigCached(plugin_device_address_t addr);
    void setEndian(int endian);
    int getCapability(usb_usecase_type_t type, plugin_device_address_t addr);
    int getMaxBitWidth(bool is_playback);
    int getMaxChannels(bool is_playback);
    unsigned int getFormatByBitWidth(int bitwidth);
    unsigned int readDefaultFormat(bool is_playback);
    unsigned int readDefaultSampleRate(bool is_playback);
    unsigned int readDefaultChannelMask(bool is_playback);
    int readSupportedConfig(dynamic_media_config_t *config, bool is_playback);
    int readBestConfig(struct qal_media_config *config,
                                    struct qal_stream_attributes *sattr,
                                    bool is_playback);
    unsigned int getMax(unsigned int a, unsigned int b);
    unsigned int getMin(unsigned int a, unsigned int b);
    static const unsigned int out_chn_mask_[MAX_SUPPORTED_CHANNEL_MASKS];
    static const unsigned int in_chn_mask_[MAX_SUPPORTED_CHANNEL_MASKS];
};

class USB : public Device
{
protected:
    static std::shared_ptr<Device> objRx;
    static std::shared_ptr<Device> objTx;
    std::vector <std::shared_ptr<USBCardConfig>> usb_card_config_list_;
    USB(struct qal_device *device, std::shared_ptr<ResourceManager> Rm);
public:
    int init(qal_param_device_connection_t device_conn);
    int deinit(qal_param_device_connection_t device_conn);
    int getDefaultConfig(qal_param_device_capability_t capability);
    int selectBestConfig(struct qal_device *dattr,
                                   struct qal_stream_attributes *sattr,
                                   bool is_playback);
    static std::shared_ptr<Device> getInstance(struct qal_device *device,
                                               std::shared_ptr<ResourceManager> Rm);
    static int32_t isSampleRateSupported(unsigned int sampleRate);
    static int32_t isChannelSupported(unsigned int numChannels);
    static int32_t isBitWidthSupported(unsigned int bitWidth);
    static int32_t checkAndUpdateBitWidth(unsigned int *bitWidth);
    static int32_t checkAndUpdateSampleRate(unsigned int *sampleRate);
    static bool isUSBOutDevice(qal_device_id_t);
    ~USB();
};


#endif //USB_H