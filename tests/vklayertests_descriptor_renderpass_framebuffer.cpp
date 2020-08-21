#include "cast_utils.h"
#include "layer_validation_tests.h"

TEST_F(VkLayerTest, AttachmentInNexusPlayer) {
    ASSERT_NO_FATAL_FAILURE(Init());
    m_errorMonitor->ExpectSuccess();

    // ASSERT_NO_FATAL_FAILURE(InitRenderTarget());
    const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    const uint32_t width = 64, height = 64;

    const VkAttachmentDescription att = {
        0,
        format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentReference ref = {
        0,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkSubpassDescription subpass = {
        0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 1, &ref, nullptr, nullptr, 0, nullptr,
    };

    const VkRenderPassCreateInfo renderPassInfo = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0, 1, &att, 1, &subpass, 0, nullptr,
    };
    VkRenderPass rp;
    ASSERT_VK_SUCCESS(vk::CreateRenderPass(device(), &renderPassInfo, nullptr, &rp));

    VkFormatProperties props;
    vk::GetPhysicalDeviceFormatProperties(m_device->phy().handle(), format, &props);
    VkImageObj img(m_device);

    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        img.Init(width, height, 1, format,
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                 VK_IMAGE_TILING_LINEAR);
    } else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        img.Init(width, height, 1, format,
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                 VK_IMAGE_TILING_OPTIMAL);
    }
    VkImageView view = img.targetView(format);

    const VkFramebufferCreateInfo fbci = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, 0, 0, rp, 1, &view, width, height, 1,
    };

    VkFramebuffer fb;
    ASSERT_VK_SUCCESS(vk::CreateFramebuffer(device(), &fbci, nullptr, &fb));
    ASSERT_VK_SUCCESS(vk::CreateFramebuffer(device(), &fbci, nullptr, &m_framebuffer));

    const VkClearValue clear = {m_clear_color};

    printf("m_framebuffer\n");

    const VkRenderPassBeginInfo rp_begin_info = {
        //VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, rp, fb, {{0, 0}, {width, height}}, 1, &clear};
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, rp, m_framebuffer, {{0, 0}, {width, height}}, 1, &clear};

    m_commandBuffer->begin();
    m_commandBuffer->BeginRenderPass(rp_begin_info);

    m_errorMonitor->VerifyNotFound();
}
