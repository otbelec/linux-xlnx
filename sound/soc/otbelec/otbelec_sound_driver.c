// SPDX-License-Identifier: GPL-2.0
/*
 * OTBelec ALSA sound card driver (machine driver).
 *
 * Limitations:
 *  - Only supports fixed mclk of 24.576 MHz.
 * 
 * Copyright (C) 2019 OTBelec
 */

#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/module.h>
#include <linux/of_device.h>

#include "../xilinx/xlnx_snd_common.h"


static int protoboard_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
    int ret;
    u32 num_ch, sample_rate, bit_depth;
    int clk_div;
    struct pl_card_data *prv;
    struct snd_soc_pcm_runtime *rtd = substream->private_data;

    num_ch = params_channels(params);
    sample_rate = params_rate(params);
    bit_depth = params_width(params);

    if (num_ch != 2) //TODO: xilinx formatter driver is missing support for multiple channels.
        return -EINVAL;

    prv = snd_soc_card_get_drvdata(rtd->card);

    /* Set Fs Multiplier value used by xilinx audio-formatter. */
    switch (sample_rate) {
        case 32000:
            prv->mclk_ratio = 768; //TODO: ratio depends on mclk freq
            break;
        case 48000:
            prv->mclk_ratio = 512;
            break;
        case 96000:
            prv->mclk_ratio = 256;
            break;
        case 192000:
            prv->mclk_ratio = 128;
            break;
        case 384000:
            prv->mclk_ratio = 64;
            break;
        case 768000:
            prv->mclk_ratio = 32;
            break;
        default:
            return -EINVAL;
    }

    /* Set SCLK Divider value used by xilinx i2s-transmitter. */
    clk_div = DIV_ROUND_UP(prv->mclk_ratio, 2 * num_ch * bit_depth);
    ret = snd_soc_dai_set_clkdiv(rtd->cpu_dai, 0, clk_div);

    return ret;
}

static const struct snd_soc_ops protoboard_snd_ops = {
    .hw_params = protoboard_hw_params,
};

static struct snd_soc_dai_link protoboard_dai_link_dac = {
    .name = "i2s-link-dac",
    .stream_name = "pcm-stream-dac",
    .codec_dai_name = "ak4458-aif",
    .dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBS_CFS, //format info to codec
    .platform_name = "xlnx_formatter_pcm",
    .ops = &protoboard_snd_ops,
};

/*
static struct snd_soc_dai_link protoboard_dai_link_adc = {
    .name = "i2s-link-adc",
    .stream_name = "pcm-stream-adc",
    .cpu_name = "xlnx_i2s",
    .cpu_dai_name = "xlnx_i2s_capture",
    .codec_dai_name = "ak5558-aif",
    .platform_name = "xlnx_formatter_pcm",
    .ops = &protoboard_snd_ops,
};
*/

static struct snd_soc_card protoboard_snd_card = {
    .name = "protoboard-snd-card",
    .owner = THIS_MODULE,
    .dai_link = &protoboard_dai_link_dac,
    .num_links = 1,
};

static int otbelec_snd_probe(struct platform_device *pdev)
{
    int ret;
    struct snd_soc_card *card = &protoboard_snd_card;
    struct snd_soc_dai_link *dai_link = &protoboard_dai_link_dac;
    struct pl_card_data *prv;
    struct device_node *phandle;

    card->dev = &pdev->dev;

    /* Attach PCM Audio Formatter using device tree lookup */


    /* Attach i2s driver using device tree lookup */
    phandle = of_parse_phandle(card->dev->of_node, "i2s-tx", 0);
    dai_link->cpu_of_node = phandle;

    /* Attach DAC using device tree lookup */
    phandle = of_parse_phandle(card->dev->of_node, "dac", 0);
    dai_link->codec_of_node = phandle;

    /* Attach ADC to I2S capture using device tree lookup */


    /* mclk properties as custom data shared with xilinx audio formatter */
    prv = devm_kzalloc(card->dev, sizeof(struct pl_card_data), GFP_KERNEL);
    if (!prv)
        return -ENOMEM;
    prv->mclk_val = 24576000; //TODO: get mclk freq from device tree.

    ret = snd_soc_register_card(card);
    if (ret)
    {
        dev_err(&pdev->dev, "protoboard register sound card failed!\n");
        return ret;
    }

    dev_info(&pdev->dev, "otbelec sound card driver probe successful\n");

    return 0;
}

static int otbelec_snd_remove(struct platform_device *pdev)
{
    struct snd_soc_card *card = &protoboard_snd_card;

    snd_soc_unregister_card(card);

    return 0;
}

static const struct of_device_id otbelec_sound_driver_id = {
	.compatible = "otbelec,sound-driver",
};

MODULE_DEVICE_TABLE(of, &otbelec_sound_driver_id);

static struct platform_driver otbelec_sound_driver = {
    .driver = {
        .name = "otbelec-sound-driver",
		.of_match_table = &otbelec_sound_driver_id,
    },
    .probe = otbelec_snd_probe,
    .remove = otbelec_snd_remove,
};


module_platform_driver(otbelec_sound_driver);

MODULE_DESCRIPTION("OTBelec ALSA sound card driver.");
MODULE_AUTHOR("Vegard Hella");
MODULE_LICENSE("GPL v2");