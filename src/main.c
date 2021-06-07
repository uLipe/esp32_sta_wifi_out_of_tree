/*
 * Copyright (c) 2020 Espressif Systems (Shanghai) Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <esp_wifi.h>
#include <esp_timer.h>
#include <esp_event.h>

#include <net/net_if.h>
#include <net/net_core.h>
#include <net/net_context.h>
#include <net/net_mgmt.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(esp32_wifi_sta, LOG_LEVEL_DBG);

static struct net_mgmt_event_callback dhcp_cb;

static void handler_cb(struct net_mgmt_event_callback *cb,
		    uint32_t mgmt_event, struct net_if *iface)
{
	if (mgmt_event != NET_EVENT_IPV4_DHCP_BOUND) {
		return;
	}

	char buf[NET_IPV4_ADDR_LEN];

	LOG_INF("Your address: %s",
		log_strdup(net_addr_ntop(AF_INET,
				   &iface->config.dhcpv4.requested_ip,
				   buf, sizeof(buf))));
	LOG_INF("Lease time: %u seconds",
			iface->config.dhcpv4.lease_time);
	LOG_INF("Subnet: %s",
		log_strdup(net_addr_ntop(AF_INET,
					&iface->config.ip.ipv4->netmask,
					buf, sizeof(buf))));
	LOG_INF("Router: %s",
		log_strdup(net_addr_ntop(AF_INET,
						&iface->config.ip.ipv4->gw,
						buf, sizeof(buf))));
}

void main(void)
{
	struct net_if *iface;

	/* Registra o callback responsavel por capturar eventos
	 * da stack tcp/ip
	 */
	net_mgmt_init_event_callback(&dhcp_cb, handler_cb,
				     NET_EVENT_IPV4_DHCP_BOUND);

	net_mgmt_add_event_callback(&dhcp_cb);

	/* Pede a interface de rede padrao da stack TCP/IP
	 * Nesse caso o WiFI do esp32
	 */
	iface = net_if_get_default();
	if (!iface) {
		LOG_ERR("wifi interface not available");
		return;
	}

	/* Nessa demo, vamos pedir um endereço IP pelo DHCP */
	net_dhcpv4_start(iface);

	/* As linhas abaixo referem-se a configuração do 
	 * wifi em termos de ssid e senha
	 */
	if (!IS_ENABLED(CONFIG_ESP32_WIFI_STA_AUTO)) {
		wifi_config_t wifi_config = {
			.sta = {
				.ssid = CONFIG_ESP32_WIFI_SSID,
				.password = CONFIG_ESP32_WIFI_PASSWORD,
			},
		};

		/* Finalmente chamamos as funções especificas do 
		 * driver de wifi do ESP32
		 */
		esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);

		/* configuramos */
		ret |= esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
		
		/* e pedimos a conexao para o AP desejado */
		ret |= esp_wifi_connect();
		if (ret != ESP_OK) {
			LOG_ERR("connection failed");
		}
	}
}
