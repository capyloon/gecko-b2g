/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict";

const { XPCOMUtils } = ChromeUtils.import(
  "resource://gre/modules/XPCOMUtils.jsm"
);

var WAP_CONSTS = ChromeUtils.import("resource://gre/modules/wap_consts.js");

const lazy = {};

XPCOMUtils.defineLazyGetter(lazy, "RIL", function () {
  let obj = ChromeUtils.import("resource://gre/modules/ril_consts.js");
  return obj;
});

var RIL_DEBUG = ChromeUtils.import(
  "resource://gre/modules/ril_consts_debug.js"
);

//const GONK_SMSSERVICE_CONTRACTID = "@mozilla.org/sms/gonksmsservice;1";
const GONK_SMSSERVICE_CID = Components.ID(
  "{f9b9b5e2-73b4-11e4-83ff-a33e27428c86}"
);

const NS_XPCOM_SHUTDOWN_OBSERVER_ID = "xpcom-shutdown";
const NS_PREFBRANCH_PREFCHANGE_TOPIC_ID = "nsPref:changed";

const kPrefLastKnownSimMcc = "ril.lastKnownSimMcc";

const kDiskSpaceWatcherObserverTopic = "disk-space-watcher";

const kSmsReceivedObserverTopic = "sms-received";
const kSilentSmsReceivedObserverTopic = "silent-sms-received";
const kSmsSendingObserverTopic = "sms-sending";
const kSmsSentObserverTopic = "sms-sent";
const kSmsFailedObserverTopic = "sms-failed";
const kSmsDeliverySuccessObserverTopic = "sms-delivery-success";
const kSmsDeliveryErrorObserverTopic = "sms-delivery-error";
const kSmsDeletedObserverTopic = "sms-deleted";
const kSmsSuplInitObserverTopic = "sms-supl-init";

const kSettingsRequestStatusReport = "ril.sms.requestStatusReport.enabled";
const kSettingsDefaukltServiceId = "ril.sms.defaultServiceId";

const DOM_MOBILE_MESSAGE_DELIVERY_RECEIVED = "received";
const DOM_MOBILE_MESSAGE_DELIVERY_SENDING = "sending";
const DOM_MOBILE_MESSAGE_DELIVERY_SENT = "sent";
const DOM_MOBILE_MESSAGE_DELIVERY_ERROR = "error";

const SMS_HANDLED_WAKELOCK_TIMEOUT = 5000;
const SMS_SUPL_INIT_PORT = 7275;

XPCOMUtils.defineLazyGetter(lazy, "gRadioInterfaces", function () {
  let ril = { numRadioInterfaces: 0 };
  try {
    ril = Cc["@mozilla.org/ril;1"].getService(Ci.nsIRadioInterfaceLayer);
  } catch (e) {}

  let interfaces = [];
  for (let i = 0; i < ril.numRadioInterfaces; i++) {
    interfaces.push(ril.getRadioInterface(i));
  }
  return interfaces;
});

XPCOMUtils.defineLazyGetter(lazy, "gSmsSegmentHelper", function () {
  let { SmsSegmentHelper } = ChromeUtils.import(
    "resource://gre/modules/SmsSegmentHelper.jsm"
  );

  // Initialize enabledGsmTableTuples from current MCC.
  SmsSegmentHelper.enabledGsmTableTuples = getEnabledGsmTableTuplesFromMcc();

  return SmsSegmentHelper;
});

XPCOMUtils.defineLazyGetter(lazy, "gWAP", function () {
  let ns = ChromeUtils.import("resource://gre/modules/WapPushManager.jsm");
  return ns;
});

XPCOMUtils.defineLazyGetter(lazy, "gSmsSendingSchedulers", function () {
  return {
    _schedulers: [],
    getSchedulerByServiceId(aServiceId) {
      let scheduler = this._schedulers[aServiceId];
      if (!scheduler) {
        scheduler = this._schedulers[aServiceId] = new SmsSendingScheduler(
          aServiceId
        );
      }

      return scheduler;
    },
  };
});

XPCOMUtils.defineLazyGetter(lazy, "SIM", function () {
  return ChromeUtils.import("resource://gre/modules/simIOHelper.js");
});

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gCellBroadcastService",
  "@mozilla.org/cellbroadcast/cellbroadcastservice;1",
  "nsIGonkCellBroadcastService"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gIccService",
  "@mozilla.org/icc/iccservice;1",
  "nsIIccService"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gMobileConnectionService",
  "@mozilla.org/mobileconnection/mobileconnectionservice;1",
  "nsIMobileConnectionService"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gMobileMessageDatabaseService",
  "@mozilla.org/mobilemessage/gonkmobilemessagedatabaseservice;1",
  "nsIGonkMobileMessageDatabaseService"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gMobileMessageService",
  "@mozilla.org/mobilemessage/mobilemessageservice;1",
  "nsIMobileMessageService"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gPowerManagerService",
  "@mozilla.org/power/powermanagerservice;1",
  "nsIPowerManagerService"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gSmsMessenger",
  "@mozilla.org/ril/system-messenger-helper;1",
  "nsISmsMessenger"
);

XPCOMUtils.defineLazyServiceGetter(
  lazy,
  "gImsRegService",
  "@mozilla.org/mobileconnection/imsregservice;1",
  "nsIImsRegService"
);

ChromeUtils.defineModuleGetter(
  lazy,
  "PhoneNumberUtils",
  "resource://gre/modules/PhoneNumberUtils.jsm"
);

XPCOMUtils.defineLazyGetter(lazy, "gSettingsObserver", function () {
  let obj = ChromeUtils.import(
    "resource://gre/modules/RILSettingsObserver.jsm"
  );
  return obj;
});

var DEBUG = RIL_DEBUG.DEBUG_RIL;
function debug(s) {
  dump("SmsService: " + s);
}

function SmsService() {
  this._updateDebugFlag();
  this._silentNumbers = [];
  this._imsSmsProviders = [];
  this._initImsSms();

  this._portAddressedSmsApps = {};

  this._portAddressedSmsApps[WAP_CONSTS.WDP_PORT_PUSH] = (
    aMessage,
    aServiceId
  ) => this._handleSmsWdpPortPush(aMessage, aServiceId);
  this._portAddressedSmsApps[SMS_SUPL_INIT_PORT] = (aMessage, aServiceId) =>
    this._handleSmsSuplInitMessage(aMessage, aServiceId);

  this._receivedSmsSegmentsMap = {};

  this.settingsObserver = new lazy.gSettingsObserver.RILSettingsObserver();
  this.settingsObserver.handleSettingChanged = this.handleSettingChanged;
  this.settingsObserver
    .getSettingWithDefault(kSettingsDefaukltServiceId, 0)
    .then(setting => {
      if (setting.value < 0 || setting.value >= lazy.gRadioInterfaces.length) {
        this.smsDefaultServiceId = 0;
      } else {
        this.smsDefaultServiceId = setting.value;
      }
    });

  this.settingsObserver.addSettingObserver(kSettingsDefaukltServiceId);

  Services.prefs.addObserver(RIL_DEBUG.PREF_RIL_DEBUG_ENABLED, this);
  Services.prefs.addObserver(kPrefLastKnownSimMcc, this);
  Services.obs.addObserver(this, NS_XPCOM_SHUTDOWN_OBSERVER_ID);
  Services.obs.addObserver(this, kDiskSpaceWatcherObserverTopic);
}
SmsService.prototype = {
  classID: GONK_SMSSERVICE_CID,

  QueryInterface: ChromeUtils.generateQI([
    Ci.nsISmsService,
    Ci.nsIGonkSmsService,
    Ci.nsIObserver,
  ]),

  // An array of silent numbers.
  _silentNumbers: null,
  //AN array of ImsSmsProviders
  _imsSmsProviders: null,

  _updateDebugFlag() {
    try {
      DEBUG =
        RIL_DEBUG.DEBUG_RIL ||
        Services.prefs.getBoolPref(RIL_DEBUG.PREF_RIL_DEBUG_ENABLED);
    } catch (e) {}
  },

  _getPhoneNumber(aServiceId) {
    let number;
    // Get the proper IccInfo based on the current card type.
    try {
      let iccInfo = null;
      let baseIccInfo = this._getIccInfo(aServiceId);
      if (baseIccInfo.iccType === "ruim" || baseIccInfo.iccType === "csim") {
        iccInfo = baseIccInfo.QueryInterface(Ci.nsICdmaIccInfo);
        number = iccInfo.mdn;
      } else {
        iccInfo = baseIccInfo.QueryInterface(Ci.nsIGsmIccInfo);
        number = iccInfo.msisdn;
      }
    } catch (e) {
      if (DEBUG) {
        debug("Exception - QueryInterface failed on iccinfo for GSM/CDMA info");
      }
      return null;
    }

    return number;
  },

  _getIccInfo(aServiceId) {
    let icc = lazy.gIccService.getIccByServiceId(aServiceId);
    return icc ? icc.iccInfo : null;
  },

  _getCardState(aServiceId) {
    let icc = lazy.gIccService.getIccByServiceId(aServiceId);
    return icc ? icc.cardState : Ci.nsIIcc.CARD_STATE_UNKNOWN;
  },

  _getIccId(aServiceId) {
    let iccInfo = this._getIccInfo(aServiceId);

    if (!iccInfo) {
      return null;
    }

    return iccInfo.iccid;
  },

  // The following attributes/functions are used for acquiring/releasing the
  // CPU wake lock when the RIL handles the received SMS. Note that we need
  // a timer to bound the lock's life cycle to avoid exhausting the battery.
  _smsHandledWakeLock: null,
  _smsHandledWakeLockTimer: null,
  _acquireSmsHandledWakeLock() {
    if (!this._smsHandledWakeLock) {
      if (DEBUG) {
        debug("Acquiring a CPU wake lock for handling SMS.");
      }
      this._smsHandledWakeLock = lazy.gPowerManagerService.newWakeLock("cpu");
    }
    if (!this._smsHandledWakeLockTimer) {
      if (DEBUG) {
        debug("Creating a timer for releasing the CPU wake lock.");
      }
      this._smsHandledWakeLockTimer = Cc["@mozilla.org/timer;1"].createInstance(
        Ci.nsITimer
      );
    }
    if (DEBUG) {
      debug("Setting the timer for releasing the CPU wake lock.");
    }
    this._smsHandledWakeLockTimer.initWithCallback(
      () => this._releaseSmsHandledWakeLock(),
      SMS_HANDLED_WAKELOCK_TIMEOUT,
      Ci.nsITimer.TYPE_ONE_SHOT
    );
  },

  _releaseSmsHandledWakeLock() {
    if (DEBUG) {
      debug("Releasing the CPU wake lock for handling SMS.");
    }
    if (this._smsHandledWakeLockTimer) {
      this._smsHandledWakeLockTimer.cancel();
    }
    if (this._smsHandledWakeLock) {
      this._smsHandledWakeLock.unlock();
      this._smsHandledWakeLock = null;
    }
  },

  _convertSmsMessageClassToString(aMessageClass) {
    return lazy.RIL.GECKO_SMS_MESSAGE_CLASSES[aMessageClass] || null;
  },

  _convertSmsMessageClass(aMessageClass) {
    let index = lazy.RIL.GECKO_SMS_MESSAGE_CLASSES.indexOf(aMessageClass);

    if (index < 0) {
      throw new Error("Invalid MessageClass: " + aMessageClass);
    }

    return index;
  },

  _convertSmsDelivery(aDelivery) {
    let index = [
      DOM_MOBILE_MESSAGE_DELIVERY_RECEIVED,
      DOM_MOBILE_MESSAGE_DELIVERY_SENDING,
      DOM_MOBILE_MESSAGE_DELIVERY_SENT,
      DOM_MOBILE_MESSAGE_DELIVERY_ERROR,
    ].indexOf(aDelivery);

    if (index < 0) {
      throw new Error("Invalid Delivery: " + aDelivery);
    }

    return index;
  },

  _convertSmsDeliveryStatus(aDeliveryStatus) {
    let index = [
      lazy.RIL.GECKO_SMS_DELIVERY_STATUS_NOT_APPLICABLE,
      lazy.RIL.GECKO_SMS_DELIVERY_STATUS_SUCCESS,
      lazy.RIL.GECKO_SMS_DELIVERY_STATUS_PENDING,
      lazy.RIL.GECKO_SMS_DELIVERY_STATUS_ERROR,
    ].indexOf(aDeliveryStatus);

    if (index < 0) {
      throw new Error("Invalid DeliveryStatus: " + aDeliveryStatus);
    }

    return index;
  },

  _notifySendingError(aErrorCode, aSendingMessage, aSilent, aRequest) {
    if (aSilent || aErrorCode === Ci.nsIMobileMessageCallback.NOT_FOUND_ERROR) {
      // There is no way to modify nsISmsMessage attributes as they
      // are read only so we just create a new sms instance to send along
      // with the notification.
      aRequest.notifySendMessageFailed(
        aErrorCode,
        lazy.gMobileMessageService.createSmsMessage(
          aSendingMessage.id,
          aSendingMessage.threadId,
          aSendingMessage.iccId,
          DOM_MOBILE_MESSAGE_DELIVERY_ERROR,
          lazy.RIL.GECKO_SMS_DELIVERY_STATUS_ERROR,
          aSendingMessage.sender,
          aSendingMessage.receiver,
          aSendingMessage.body,
          aSendingMessage.messageClass,
          aSendingMessage.timestamp,
          0,
          0,
          aSendingMessage.read
        )
      );

      if (!aSilent) {
        Services.obs.notifyObservers(aSendingMessage, kSmsFailedObserverTopic);
      }
      return;
    }

    lazy.gMobileMessageDatabaseService.setMessageDeliveryByMessageId(
      aSendingMessage.id,
      null,
      DOM_MOBILE_MESSAGE_DELIVERY_ERROR,
      lazy.RIL.GECKO_SMS_DELIVERY_STATUS_ERROR,
      null,
      (aRv, aDomMessage) => {
        let smsMessage = null;
        try {
          smsMessage = aDomMessage.QueryInterface(Ci.nsISmsMessage);
        } catch (e) {}
        // TODO bug 832140 handle !Components.isSuccessCode(aRv)
        this._broadcastSmsSystemMessage(
          Ci.nsISmsMessenger.NOTIFICATION_TYPE_SENT_FAILED,
          smsMessage
        );
        aRequest.notifySendMessageFailed(aErrorCode, smsMessage);
        Services.obs.notifyObservers(smsMessage, kSmsFailedObserverTopic);
      }
    );
  },

  /**
   * Schedule the sending request.
   */
  _scheduleSending(aServiceId, aDomMessage, aSilent, aOptions, aRequest) {
    lazy.gSmsSendingSchedulers.getSchedulerByServiceId(aServiceId).schedule({
      messageId: aDomMessage.id,
      onSend: () => {
        if (DEBUG) {
          debug(
            "onSend: messageId=" + aDomMessage.id + ", serviceId=" + aServiceId
          );
        }
        this._sendToTheAir(
          aServiceId,
          aDomMessage,
          aSilent,
          aOptions,
          aRequest
        );
      },
      onCancel: aErrorCode => {
        if (DEBUG) {
          debug("onCancel: " + aErrorCode);
        }
        this._notifySendingError(aErrorCode, aDomMessage, aSilent, aRequest);
      },
    });
  },

  /**
   * Send a SMS message to the modem.
   */
  _sendToTheAir(aServiceId, aDomMessage, aSilent, aOptions, aRequest) {
    // Keep current SMS message info for sent/delivered notifications
    let sentMessage = aDomMessage;
    let requestStatusReport = aOptions.requestStatusReport;

    // Retry count for GECKO_ERROR_SMS_SEND_FAIL_RETRY
    if (!aOptions.retryCount) {
      aOptions.retryCount = 0;
    }
    if (DEBUG) {
      debug("_sendToTheAir aOptions: " + JSON.stringify(aOptions));
    }

    let sendSMSCallback = aResponse => {
      // Failed to send SMS out.
      if (aResponse.errorMsg) {
        let error = Ci.nsIMobileMessageCallback.UNKNOWN_ERROR;
        if (aResponse.errorMsg === lazy.RIL.GECKO_ERROR_RADIO_NOT_AVAILABLE) {
          error = Ci.nsIMobileMessageCallback.NO_SIGNAL_ERROR;
        } else if (
          aResponse.errorMsg === lazy.RIL.GECKO_ERROR_FDN_CHECK_FAILURE
        ) {
          error = Ci.nsIMobileMessageCallback.FDN_CHECK_ERROR;
        } else if (
          aResponse.errorMsg === lazy.RIL.GECKO_ERROR_SMS_SEND_FAIL_RETRY &&
          aOptions.retryCount < lazy.RIL.SMS_RETRY_MAX
        ) {
          aOptions.retryCount++;
          this._scheduleSending(
            aServiceId,
            aDomMessage,
            aSilent,
            aOptions,
            aRequest
          );
          return false;
        }

        this._notifySendingError(error, sentMessage, aSilent, aRequest);
        return false;
      } // End of send failure.

      //Failed to send IMS SMS out
      if (
        aResponse.status &&
        aResponse.status != Ci.nsIImsMMTelFeature.SEND_STATUS_OK
      ) {
        if (DEBUG) {
          debug("_sendToTheAir: SendImsSms failed " + aResponse.status);
        }
        if (
          aResponse.status === Ci.nsIImsMMTelFeature.SEND_STATUS_ERROR_FALLBACK
        ) {
          //IMS Fallback to CS
          if (DEBUG) {
            debug("_sendToTheAir: Resend and Fallback to CS ");
          }
          aOptions.retryFallback = true;
          this._scheduleSending(
            aServiceId,
            aDomMessage,
            aSilent,
            aOptions,
            aRequest
          );
          return false;
        } else if (
          aResponse.status === Ci.nsIImsMMTelFeature.SEND_STATUS_ERROR_RETRY &&
          aOptions.retryCount < lazy.RIL.SMS_RETRY_MAX
        ) {
          aOptions.retryCount++;
          this._scheduleSending(
            aServiceId,
            aDomMessage,
            aSilent,
            aOptions,
            aRequest
          );
          return false;
        }
        let error = Ci.nsIMobileMessageCallback.UNKNOWN_ERROR;
        if (
          aResponse.reason ===
          Ci.nsIImsMMTelFeature.RESULT_ERROR_FDN_CHECK_FAILURE
        ) {
          error = Ci.nsIMobileMessageCallback.FDN_CHECK_ERROR;
        } else if (
          aResponse.reason === Ci.nsIImsMMTelFeature.RESULT_ERROR_NO_SERVICE ||
          aResponse.reason === Ci.nsIImsMMTelFeature.RESULT_RADIO_NOT_AVAILABLE
        ) {
          error = Ci.nsIMobileMessageCallback.NO_SIGNAL_ERROR;
        }
        this._notifySendingError(error, sentMessage, aSilent, aRequest);
        return false;
      } // End of IMS send failure.

      // Message was sent to SMSC.
      if (!aResponse.deliveryStatus) {
        if (aSilent) {
          // There is no way to modify nsISmsMessage attributes as they
          // are read only so we just create a new sms instance to send along
          // with the notification.
          aRequest.notifyMessageSent(
            lazy.gMobileMessageService.createSmsMessage(
              sentMessage.id,
              sentMessage.threadId,
              sentMessage.iccId,
              DOM_MOBILE_MESSAGE_DELIVERY_SENT,
              sentMessage.deliveryStatus,
              sentMessage.sender,
              sentMessage.receiver,
              sentMessage.body,
              sentMessage.messageClass,
              sentMessage.timestamp,
              Date.now(),
              0,
              sentMessage.read
            )
          );
          // We don't wait for SMS-STATUS-REPORT for silent one.
          return false;
        }

        lazy.gMobileMessageDatabaseService.setMessageDeliveryByMessageId(
          sentMessage.id,
          null,
          DOM_MOBILE_MESSAGE_DELIVERY_SENT,
          sentMessage.deliveryStatus,
          null,
          (aRv, aDomMessage) => {
            let smsMessage = null;
            try {
              smsMessage = aDomMessage.QueryInterface(Ci.nsISmsMessage);
            } catch (e) {}
            // TODO bug 832140 handle !Components.isSuccessCode(aRv)

            if (requestStatusReport) {
              // Update the sentMessage and wait for the status report.
              sentMessage = smsMessage;
            }

            this._broadcastSmsSystemMessage(
              Ci.nsISmsMessenger.NOTIFICATION_TYPE_SENT,
              smsMessage
            );
            aRequest.notifyMessageSent(smsMessage);
            Services.obs.notifyObservers(smsMessage, kSmsSentObserverTopic);
          }
        );

        // Keep this callback if we have status report waiting.
        return requestStatusReport;
      } // End of Message Sent to SMSC.

      // Got valid deliveryStatus for the delivery to the remote party when
      // the status report is requested.
      lazy.gMobileMessageDatabaseService.setMessageDeliveryByMessageId(
        sentMessage.id,
        null,
        sentMessage.delivery,
        aResponse.deliveryStatus,
        null,
        (aRv, aDomMessage) => {
          let smsMessage = null;
          try {
            smsMessage = aDomMessage.QueryInterface(Ci.nsISmsMessage);
          } catch (e) {}
          // TODO bug 832140 handle !Components.isSuccessCode(aRv)

          let [topic, notificationType] =
            aResponse.deliveryStatus ==
            lazy.RIL.GECKO_SMS_DELIVERY_STATUS_SUCCESS
              ? [
                  kSmsDeliverySuccessObserverTopic,
                  Ci.nsISmsMessenger.NOTIFICATION_TYPE_DELIVERY_SUCCESS,
                ]
              : [
                  kSmsDeliveryErrorObserverTopic,
                  Ci.nsISmsMessenger.NOTIFICATION_TYPE_DELIVERY_ERROR,
                ];

          // Broadcasting a "sms-delivery-success/sms-delivery-error" system
          // message to open apps.
          this._broadcastSmsSystemMessage(notificationType, smsMessage);

          // Notifying observers the delivery status is updated.
          Services.obs.notifyObservers(smsMessage, topic);
        }
      );

      // Send transaction has ended completely.
      return false;
    };

    if (this._isIms(aServiceId) && !aOptions.retryFallback) {
      this._imsSmsProviders[aServiceId].sendSms(aOptions, sendSMSCallback);
      return;
    }

    lazy.gRadioInterfaces[aServiceId].sendWorkerMessage(
      "sendSMS",
      aOptions,
      sendSMSCallback
    );
  },

  /**
   * A helper to broadcast the system message to launch registered apps
   * like Costcontrol, Notification and Message app... etc.
   *
   * @param aNotificationType
   *        Ci.nsISmsMessenger.NOTIFICATION_TYPE_*.
   * @param aDomMessage
   *        The nsISmsMessage object.
   */
  _broadcastSmsSystemMessage(aNotificationType, aDomMessage) {
    if (DEBUG) {
      debug("Broadcasting the SMS system message: " + aNotificationType);
    }

    // Sadly we cannot directly broadcast the aDomMessage object
    // because the system message mechamism will rewrap the object
    // based on the content window, which needs to know the properties.
    try {
      lazy.gSmsMessenger.notifySms(
        aNotificationType,
        aDomMessage.id,
        aDomMessage.threadId,
        aDomMessage.iccId,
        this._convertSmsDelivery(aDomMessage.delivery),
        this._convertSmsDeliveryStatus(aDomMessage.deliveryStatus),
        aDomMessage.sender,
        aDomMessage.receiver,
        aDomMessage.body,
        this._convertSmsMessageClass(aDomMessage.messageClass),
        aDomMessage.timestamp,
        aDomMessage.sentTimestamp,
        aDomMessage.deliveryTimestamp,
        aDomMessage.read
      );
    } catch (e) {
      if (DEBUG) {
        debug("Failed to _broadcastSmsSystemMessage: " + e);
      }
    }
  },

  /**
   * Helper for processing received multipart SMS.
   *
   * @return null for handled segments, and an object containing full message
   *         body/data once all segments are received.
   *
   * |_receivedSmsSegmentsMap|:
   *   Hash map for received multipart sms fragments. Messages are hashed with
   *   its sender address and concatenation reference number. Three additional
   *   attributes `segmentMaxSeq`, `receivedSegments`, `segments` are inserted.
   */
  _receivedSmsSegmentsMap: null,
  _processReceivedSmsSegment(aSegment) {
    // Directly replace full message body for single SMS.
    if (!(aSegment.segmentMaxSeq && aSegment.segmentMaxSeq > 1)) {
      if (
        aSegment.encoding ==
        Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET
      ) {
        aSegment.fullData = aSegment.data;
      } else {
        aSegment.fullBody = aSegment.body;
      }
      return aSegment;
    }

    // Handle Concatenation for Class 0 SMS
    let hash =
      aSegment.sender +
      ":" +
      aSegment.segmentRef +
      ":" +
      aSegment.segmentMaxSeq;
    let seq = aSegment.segmentSeq;

    let options = this._receivedSmsSegmentsMap[hash];
    if (!options) {
      options = aSegment;
      this._receivedSmsSegmentsMap[hash] = options;

      options.receivedSegments = 0;
      options.segments = [];
    } else if (options.segments[seq]) {
      if (
        options.encoding ==
          Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET &&
        options.encoding == aSegment.encoding &&
        options.segments[seq].length == aSegment.data.length &&
        options.segments[seq].every(function (aElement, aIndex) {
          return aElement == aSegment.data[aIndex];
        })
      ) {
        if (DEBUG) {
          debug("Got duplicated binary segment no: " + seq);
        }
        return null;
      }

      if (
        options.encoding !=
          Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET &&
        aSegment.encoding !=
          Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET &&
        options.segments[seq] == aSegment.body
      ) {
        if (DEBUG) {
          debug("Got duplicated text segment no: " + seq);
        }
        return null;
      }

      // Update mandatory properties to ensure that the segments could be
      // concatenated properly.
      options.encoding = aSegment.encoding;
      options.originatorPort = aSegment.originatorPort;
      options.destinationPort = aSegment.destinationPort;
      options.teleservice = aSegment.teleservice;
      // Decrease the counter for this collided segment.
      options.receivedSegments--;
    }

    if (options.receivedSegments > 0) {
      // Update received timestamp.
      options.timestamp = aSegment.timestamp;
    }

    if (
      options.encoding ==
      Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET
    ) {
      options.segments[seq] = aSegment.data;
    } else {
      options.segments[seq] = aSegment.body;
    }
    options.receivedSegments++;

    // The port information is only available in 1st segment for CDMA WAP Push.
    // If the segments of a WAP Push are not received in sequence
    // (e.g., SMS with seq == 1 is not the 1st segment received by the device),
    // we have to retrieve the port information from 1st segment and
    // save it into the cached options.
    if (
      aSegment.teleservice === lazy.RIL.PDU_CDMA_MSG_TELESERVICE_ID_WAP &&
      seq === 1
    ) {
      if (
        options.originatorPort ===
          Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID &&
        aSegment.originatorPort !==
          Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID
      ) {
        options.originatorPort = aSegment.originatorPort;
      }

      if (
        options.destinationPort ===
          Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID &&
        aSegment.destinationPort !==
          Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID
      ) {
        options.destinationPort = aSegment.destinationPort;
      }
    }

    if (options.receivedSegments < options.segmentMaxSeq) {
      if (DEBUG) {
        debug(
          "Got segment no." +
            seq +
            " of a multipart SMS: " +
            JSON.stringify(options)
        );
      }
      return null;
    }

    // Remove from map
    delete this._receivedSmsSegmentsMap[hash];

    // Rebuild full body
    if (
      options.encoding ==
      Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET
    ) {
      // Uint8Array doesn't have `concat`, so we have to merge all segements
      // by hand.
      let fullDataLen = 0;
      for (let i = 1; i <= options.segmentMaxSeq; i++) {
        fullDataLen += options.segments[i].length;
      }

      options.fullData = new Uint8Array(fullDataLen);
      for (let d = 0, i = 1; i <= options.segmentMaxSeq; i++) {
        let data = options.segments[i];
        for (let j = 0; j < data.length; j++) {
          options.fullData[d++] = data[j];
        }
      }
    } else {
      options.fullBody = options.segments.join("");
    }

    // Remove handy fields after completing the concatenation.
    delete options.receivedSegments;
    delete options.segments;

    if (DEBUG) {
      debug("Got full multipart SMS: " + JSON.stringify(options));
    }

    return options;
  },

  /**
   * Helper to purge complete message.
   *
   * We remove unnessary fields after completing the concatenation.
   */
  _purgeCompleteSmsMessage(aMessage) {
    // Purge concatenation info
    delete aMessage.segmentRef;
    delete aMessage.segmentSeq;
    delete aMessage.segmentMaxSeq;

    // Purge partial message body
    delete aMessage.data;
    delete aMessage.body;
  },

  /**
   * Handle WDP port push PDU. Constructor WDP bearer information and deliver
   * to WapPushManager.
   *
   * @param aMessage
   *        A SMS message.
   */
  _handleSmsWdpPortPush(aMessage, aServiceId) {
    if (
      aMessage.encoding !=
      Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET
    ) {
      if (DEBUG) {
        debug(
          "Got port addressed SMS but not encoded in 8-bit alphabet. Drop!"
        );
      }
      return;
    }

    let options = {
      bearer: WAP_CONSTS.WDP_BEARER_GSM_SMS_GSM_MSISDN,
      sourceAddress: aMessage.sender,
      sourcePort: aMessage.originatorPort,
      destinationAddress: this._getPhoneNumber(aServiceId),
      destinationPort: aMessage.destinationPort,
      serviceId: aServiceId,
    };
    lazy.gWAP.WapPushManager.receiveWdpPDU(
      aMessage.fullData,
      aMessage.fullData.length,
      0,
      options
    );
  },

  /**
   * Handle Supl Init Message. Constructor Ni information and notify observers
   */
  _handleSmsSuplInitMessage(aMessage, aServiceId) {
    aMessage.type = "sms";
    aMessage.receiver = this._getPhoneNumber(aServiceId);
    aMessage.body = aMessage.fullBody = aMessage.fullBody || null;
    if (DEBUG) {
      debug(
        "handleSmsSuplInit, receiver = " +
          aMessage.receiver +
          ", Message = " +
          aMessage.body
      );
    }
    Services.obs.notifyObservers(
      aMessage,
      kSmsSuplInitObserverTopic,
      aMessage.body
    );
  },

  _handleSmsDataMessage(aMessage, aServiceId) {
    try {
      lazy.gSmsMessenger.notifyDataSms(
        aServiceId,
        aMessage.iccId,
        aMessage.sender,
        this._getPhoneNumber(aServiceId),
        aMessage.originatorPort,
        aMessage.destinationPort,
        aMessage.fullData
      );
    } catch (e) {
      if (DEBUG) {
        debug("Failed to _broadcastSmsSystemMessage: " + e);
      }
    }
  },

  _handleCellbroadcastMessageReceived(aMessage, aServiceId) {
    let gonkCellBroadcastMessage = {
      QueryInterface: ChromeUtils.generateQI([Ci.nsIGonkCellBroadcastMessage]),
      gsmGeographicalScope:
        Ci.nsICellBroadcastService.GSM_GEOGRAPHICAL_SCOPE_INVALID,
      messageCode: aMessage.messageCode,
      messageId: aMessage.messageId,
      language: aMessage.language,
      body: aMessage.fullBody,
      messageClass: Ci.nsICellBroadcastService.GSM_MESSAGE_CLASS_NORMAL,
      timeStamp: Date.now(),
      cdmaServiceCategory: aMessage.serviceCategory,
      hasEtwsInfo: false,
      etwsWarningType: Ci.nsICellBroadcastService.GSM_ETWS_WARNING_INVALID,
      etwsEmergencyUserAlert: false,
      etwsPopup: false,
    };

    lazy.gCellBroadcastService.notifyMessageReceived(
      aServiceId,
      gonkCellBroadcastMessage
    );
  },

  _handleMwis(aMwi, aServiceId) {
    let service = Cc["@mozilla.org/voicemail/voicemailservice;1"].getService(
      Ci.nsIGonkVoicemailService
    );
    service.notifyStatusChanged(
      aServiceId,
      aMwi.active,
      aMwi.msgCount,
      aMwi.returnNumber,
      aMwi.returnMessage
    );

    lazy.gRadioInterfaces[aServiceId].sendWorkerMessage("updateMwis", {
      mwi: aMwi,
    });
  },

  _portAddressedSmsApps: null,
  _handleSmsReceived(aMessage, aServiceId) {
    if (DEBUG) {
      debug("_handleSmsReceived: " + JSON.stringify(aMessage));
    }

    if (aMessage.messageType == lazy.RIL.PDU_CDMA_MSG_TYPE_BROADCAST) {
      this._handleCellbroadcastMessageReceived(aMessage, aServiceId);
      return true;
    }

    // Dispatch to registered handler if application port addressing is
    // available. Note that the destination port can possibly be zero when
    // representing a UDP/TCP port.
    if (
      aMessage.destinationPort !==
      Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID
    ) {
      if (DEBUG) {
        debug("Message destinationPort = " + aMessage.destinationPort);
      }
      let handler = this._portAddressedSmsApps[aMessage.destinationPort];
      if (handler) {
        handler(aMessage, aServiceId);
      } else {
        this._handleSmsDataMessage(aMessage, aServiceId);
      }
      return true;
    }

    if (
      aMessage.encoding ==
      Ci.nsIGonkSmsService.SMS_MESSAGE_ENCODING_8BITS_ALPHABET
    ) {
      // Don't know how to handle binary data yet.
      return true;
    }

    aMessage.type = "sms";
    aMessage.sender = aMessage.sender || null;
    aMessage.receiver = this._getPhoneNumber(aServiceId);
    aMessage.body = aMessage.fullBody = aMessage.fullBody || null;

    if (this._isSilentNumber(aMessage.sender)) {
      aMessage.id = -1;
      aMessage.threadId = 0;
      aMessage.delivery = DOM_MOBILE_MESSAGE_DELIVERY_RECEIVED;
      aMessage.deliveryStatus = lazy.RIL.GECKO_SMS_DELIVERY_STATUS_SUCCESS;
      aMessage.read = false;

      let domMessage = lazy.gMobileMessageService.createSmsMessage(
        aMessage.id,
        aMessage.threadId,
        aMessage.iccId,
        aMessage.delivery,
        aMessage.deliveryStatus,
        aMessage.sender,
        aMessage.receiver,
        aMessage.body,
        aMessage.messageClass,
        aMessage.timestamp,
        aMessage.sentTimestamp,
        0,
        aMessage.read
      );

      Services.obs.notifyObservers(domMessage, kSilentSmsReceivedObserverTopic);
      return true;
    }

    if (aMessage.mwiPresent) {
      let mwi = {
        discard: aMessage.mwiDiscard,
        msgCount: aMessage.mwiMsgCount,
        active: aMessage.mwiActive,
        returnNumber: aMessage.sender || null,
        returnMessage: aMessage.fullBody || null,
      };

      this._handleMwis(mwi, aServiceId);

      // Dicarded MWI comes without text body.
      // Hence, we discard it here after notifying the MWI status.
      if (aMessage.mwiDiscard) {
        return true;
      }
    }

    let notifyReceived = (aRv, aDomMessage) => {
      let smsMessage = null;
      try {
        smsMessage = aDomMessage.QueryInterface(Ci.nsISmsMessage);
      } catch (e) {
        if (DEBUG) {
          debug("Could not get nsISmsMessage");
        }
      }

      let success = Components.isSuccessCode(aRv);
      this._sendAckSms(aRv, aMessage, aServiceId);

      if (!success) {
        // At this point we could send a message to content to notify the user
        // that storing an incoming SMS failed, most likely due to a full disk.
        if (DEBUG) {
          debug("Could not store SMS, error code " + aRv);
        }
        return;
      }

      this._broadcastSmsSystemMessage(
        Ci.nsISmsMessenger.NOTIFICATION_TYPE_RECEIVED,
        smsMessage
      );
      Services.obs.notifyObservers(smsMessage, kSmsReceivedObserverTopic);
    };

    if (
      aMessage.messageClass !=
      lazy.RIL.GECKO_SMS_MESSAGE_CLASSES[lazy.RIL.PDU_DCS_MSG_CLASS_0]
    ) {
      lazy.gMobileMessageDatabaseService.saveReceivedMessage(
        aMessage,
        notifyReceived
      );
    } else {
      aMessage.id = -1;
      aMessage.threadId = 0;
      aMessage.delivery = DOM_MOBILE_MESSAGE_DELIVERY_RECEIVED;
      aMessage.deliveryStatus = lazy.RIL.GECKO_SMS_DELIVERY_STATUS_SUCCESS;
      aMessage.read = false;

      let domMessage = lazy.gMobileMessageService.createSmsMessage(
        aMessage.id,
        aMessage.threadId,
        aMessage.iccId,
        aMessage.delivery,
        aMessage.deliveryStatus,
        aMessage.sender,
        aMessage.receiver,
        aMessage.body,
        aMessage.messageClass,
        aMessage.timestamp,
        aMessage.sentTimestamp,
        0,
        aMessage.read
      );

      notifyReceived(Cr.NS_OK, domMessage);
    }

    // SMS ACK will be sent in notifyReceived. Return false here.
    return false;
  },

  /**
   * Handle ACK response of received SMS.
   */
  _sendAckSms(aRv, aMessage, aServiceId) {
    if (
      aMessage.messageClass ===
      lazy.RIL.GECKO_SMS_MESSAGE_CLASSES[lazy.RIL.PDU_DCS_MSG_CLASS_2]
    ) {
      return;
    }

    let result = lazy.RIL.PDU_FCS_OK;
    if (!Components.isSuccessCode(aRv)) {
      if (DEBUG) {
        debug("Failed to handle received sms: " + aRv);
      }
      result =
        aRv === Cr.NS_ERROR_FILE_NO_DEVICE_SPACE
          ? lazy.RIL.PDU_FCS_MEMORY_CAPACITY_EXCEEDED
          : lazy.RIL.PDU_FCS_UNSPECIFIED;
    }

    if (DEBUG) {
      debug("_sendAckSms Message: " + JSON.stringify(aMessage));
    }
    if (aMessage.imsMessage) {
      this._imsSmsProviders[aServiceId].acknowledgeSms(result);
    } else {
      lazy.gRadioInterfaces[aServiceId].sendWorkerMessage("ackSMS", {
        result,
      });
    }
  },

  /**
   * Report SMS storage status to modem.
   *
   * Note: GonkDiskSpaceWatcher repeats the notification every 5 seconds when
   *       storage is full.
   *       Report status to modem only when the availability is changed.
   *       Set |_smsStorageAvailable| to |null| to ensure the first run after
   *       bootup.
   */
  _smsStorageAvailable: null,
  _reportSmsMemoryStatus(aIsAvailable) {
    if (this._smsStorageAvailable !== aIsAvailable) {
      this._smsStorageAvailable = aIsAvailable;
      for (
        let serviceId = 0;
        serviceId < lazy.gRadioInterfaces.length;
        serviceId++
      ) {
        lazy.gRadioInterfaces[serviceId].sendWorkerMessage(
          "reportSmsMemoryStatus",
          {
            isAvailable: aIsAvailable,
          }
        );
      }
    }
  },

  _isSilentNumber(aNumber) {
    return this._silentNumbers.includes(aNumber);
  },

  /**
   * nsISmsService interface
   */
  smsDefaultServiceId: 0,

  getSegmentInfoForText(aText, aRequest) {
    let strict7BitEncoding = Services.prefs.getBoolPref(
      "dom.sms.strict7BitEncoding",
      false
    );

    let options = lazy.gSmsSegmentHelper.fragmentText(
      aText,
      null,
      strict7BitEncoding
    );
    let charsInLastSegment;
    if (options.segmentMaxSeq) {
      let lastSegment = options.segments[options.segmentMaxSeq - 1];
      charsInLastSegment = lastSegment.encodedBodyLength;
      if (options.dcs == lazy.RIL.PDU_DCS_MSG_CODING_16BITS_ALPHABET) {
        // In UCS2 encoding, encodedBodyLength is in octets.
        charsInLastSegment /= 2;
      }
    } else {
      charsInLastSegment = 0;
    }

    aRequest.notifySegmentInfoForTextGot(
      options.segmentMaxSeq,
      options.segmentChars,
      options.segmentChars - charsInLastSegment
    );
  },

  send(aServiceId, aNumber, aMessage, aSilent, aRequest) {
    if (aServiceId > lazy.gRadioInterfaces.length - 1) {
      throw Components.Exception("", Cr.NS_ERROR_INVALID_ARG);
    }
    if (DEBUG) {
      debug("start to send sms");
    }
    let strict7BitEncoding = Services.prefs.getBoolPref(
      "dom.sms.strict7BitEncoding",
      false
    );

    let options = lazy.gSmsSegmentHelper.fragmentText(
      aMessage,
      null,
      strict7BitEncoding
    );

    options.number = lazy.PhoneNumberUtils.normalize(aNumber);
    this.settingsObserver
      .getSettingWithDefault(kSettingsRequestStatusReport, true)
      .then(setting => {
        let requestStatusReport = setting.value;
        options.requestStatusReport = requestStatusReport && !aSilent;

        let sendingMessage = {
          type: "sms",
          sender: this._getPhoneNumber(aServiceId),
          receiver: aNumber,
          body: aMessage,
          deliveryStatusRequested: options.requestStatusReport,
          timestamp: Date.now(),
          iccId: this._getIccId(aServiceId),
        };

        let saveSendingMessageCallback = (aRv, aDomMessage) => {
          let smsMessage = null;
          try {
            smsMessage = aDomMessage.QueryInterface(Ci.nsISmsMessage);
          } catch (e) {}

          if (!Components.isSuccessCode(aRv)) {
            if (DEBUG) {
              debug("Error! Fail to save sending message! aRv = " + aRv);
            }
            this._broadcastSmsSystemMessage(
              Ci.nsISmsMessenger.NOTIFICATION_TYPE_SENT_FAILED,
              smsMessage
            );
            aRequest.notifySendMessageFailed(
              lazy.gMobileMessageDatabaseService.translateCrErrorToMessageCallbackError(
                aRv
              ),
              smsMessage
            );
            Services.obs.notifyObservers(smsMessage, kSmsFailedObserverTopic);
            return;
          }

          if (!aSilent) {
            Services.obs.notifyObservers(smsMessage, kSmsSendingObserverTopic);
          }

          let connection =
            lazy.gMobileConnectionService.getItemByServiceId(aServiceId);
          // If the radio is disabled or the SIM card is not ready, just directly
          // return with the corresponding error code.
          let errorCode;
          let radioState = connection && connection.radioState;

          if (!lazy.PhoneNumberUtils.isPlainPhoneNumber(options.number)) {
            if (DEBUG) {
              debug(
                "Error! Address is invalid when sending SMS: " + options.number
              );
            }
            errorCode = Ci.nsIMobileMessageCallback.INVALID_ADDRESS_ERROR;
          } else if (
            radioState == Ci.nsIMobileConnection.MOBILE_RADIO_STATE_UNKNOWN ||
            (radioState == Ci.nsIMobileConnection.MOBILE_RADIO_STATE_DISABLED &&
              !lazy.gSmsSendingSchedulers
                .getSchedulerByServiceId(aServiceId)
                .isVoWifiConnected())
          ) {
            if (DEBUG) {
              debug("Error! Radio is disabled when sending SMS.");
            }
            errorCode = Ci.nsIMobileMessageCallback.RADIO_DISABLED_ERROR;
          } else if (
            this._getCardState(aServiceId) != Ci.nsIIcc.CARD_STATE_READY
          ) {
            if (DEBUG) {
              debug("Error! SIM card is not ready when sending SMS.");
            }
            errorCode = Ci.nsIMobileMessageCallback.NO_SIM_CARD_ERROR;
          }
          if (errorCode) {
            this._notifySendingError(errorCode, smsMessage, aSilent, aRequest);
            return;
          }

          this._scheduleSending(
            aServiceId,
            smsMessage,
            aSilent,
            options,
            aRequest
          );
        }; // End of |saveSendingMessageCallback|.

        // Don't save message into DB for silent SMS.
        if (aSilent) {
          let delivery = DOM_MOBILE_MESSAGE_DELIVERY_SENDING;
          let deliveryStatus = lazy.RIL.GECKO_SMS_DELIVERY_STATUS_PENDING;
          let domMessage = lazy.gMobileMessageService.createSmsMessage(
            -1, // id
            0, // threadId
            sendingMessage.iccId,
            delivery,
            deliveryStatus,
            sendingMessage.sender,
            sendingMessage.receiver,
            sendingMessage.body,
            "normal", // message class
            sendingMessage.timestamp,
            0,
            0,
            false
          );
          saveSendingMessageCallback(Cr.NS_OK, domMessage);
          return;
        }

        lazy.gMobileMessageDatabaseService.saveSendingMessage(
          sendingMessage,
          saveSendingMessageCallback
        );
      });
  },

  addSilentNumber(aNumber) {
    if (this._isSilentNumber(aNumber)) {
      throw Components.Exception("", Cr.NS_ERROR_UNEXPECTED);
    }

    this._silentNumbers.push(aNumber);
  },

  removeSilentNumber(aNumber) {
    let index = this._silentNumbers.indexOf(aNumber);
    if (index < 0) {
      throw Components.Exception("", Cr.NS_ERROR_INVALID_ARG);
    }

    this._silentNumbers.splice(index, 1);
  },

  getSmscAddress(aServiceId, aRequest) {
    if (aServiceId > lazy.gRadioInterfaces.length - 1) {
      throw Components.Exception("", Cr.NS_ERROR_INVALID_ARG);
    }

    lazy.gRadioInterfaces[aServiceId].sendWorkerMessage(
      "getSmscAddress",
      null,
      aResponse => {
        if (!aResponse.errorMsg) {
          aRequest.notifyGetSmscAddress(
            aResponse.smscAddress,
            aResponse.typeOfNumber,
            aResponse.numberPlanIdentification
          );
        } else {
          aRequest.notifyGetSmscAddressFailed(
            Ci.nsIMobileMessageCallback.NOT_FOUND_ERROR
          );
        }
      }
    );
  },

  setSmscAddress(
    aServiceId,
    aNumber,
    aTypeOfNumber,
    aNumberPlanIdentification,
    aRequest
  ) {
    if (aServiceId > lazy.gRadioInterfaces.length - 1) {
      throw Components.Exception("", Cr.NS_ERROR_INVALID_ARG);
    }

    let options = {
      smscAddress: aNumber,
      typeOfNumber: aTypeOfNumber,
      numberPlanIdentification: aNumberPlanIdentification,
    };

    lazy.gRadioInterfaces[aServiceId].sendWorkerMessage(
      "setSmscAddress",
      options,
      aResponse => {
        if (!aResponse.errorMsg) {
          aRequest.notifySetSmscAddress();
        } else {
          aRequest.notifySetSmscAddressFailed(
            Ci.nsIMobileMessageCallback.INVALID_ADDRESS_ERROR
          );
        }
      }
    );
  },

  /**
   * nsIGonkSmsService interface
   */
  notifyMessageReceived(aServiceId, aSmsMessage, aData, aDataLength) {
    this._acquireSmsHandledWakeLock();

    let segment = {};
    segment.iccId = this._getIccId(aServiceId);
    segment.SMSC = aSmsMessage.smsc;
    segment.sentTimestamp = aSmsMessage.sentTimestamp;
    segment.timestamp = Date.now();
    segment.sender = aSmsMessage.sender;
    segment.pid = aSmsMessage.pid;
    segment.encoding = aSmsMessage.encoding;
    segment.messageClass = this._convertSmsMessageClassToString(
      aSmsMessage.messageClass
    );
    segment.language = aSmsMessage.language;
    segment.segmentRef = aSmsMessage.segmentRef;
    segment.segmentSeq = aSmsMessage.segmentSeq;
    segment.segmentMaxSeq = aSmsMessage.segmentMaxSeq;
    segment.originatorPort = aSmsMessage.originatorPort;
    segment.destinationPort = aSmsMessage.destinationPort;
    segment.imsMessage = aSmsMessage.imsMessage;
    segment.mwiPresent = aSmsMessage.mwiPresent;
    segment.mwiDiscard = aSmsMessage.mwiDiscard;
    segment.mwiMsgCount = aSmsMessage.mwiMsgCount;
    segment.mwiActive = aSmsMessage.mwiActive;
    segment.messageType = aSmsMessage.cdmaMessageType;
    segment.teleservice = aSmsMessage.cdmaTeleservice;
    segment.serviceCategory = aSmsMessage.cdmaServiceCategory;
    segment.body = aSmsMessage.body;
    segment.data = aData && aDataLength > 0 ? aData : null;

    let isMultipart = segment.segmentMaxSeq && segment.segmentMaxSeq > 1;
    let messageClass = segment.messageClass;

    let handleReceivedAndAck = (aRvOfIncompleteMsg, aCompleteMessage) => {
      if (aCompleteMessage) {
        this._purgeCompleteSmsMessage(aCompleteMessage);
        if (this._handleSmsReceived(aCompleteMessage, aServiceId)) {
          this._sendAckSms(Cr.NS_OK, aCompleteMessage, aServiceId);
        }
        // else Ack will be sent after further process in _handleSmsReceived.
      } else {
        this._sendAckSms(aRvOfIncompleteMsg, segment, aServiceId);
      }
    };

    // No need to access SmsSegmentStore for Class 0 SMS and Single SMS.
    if (
      !isMultipart ||
      messageClass ==
        lazy.RIL.GECKO_SMS_MESSAGE_CLASSES[lazy.RIL.PDU_DCS_MSG_CLASS_0]
    ) {
      // `When a mobile terminated message is class 0 and the MS has the
      // capability of displaying short messages, the MS shall display the
      // message immediately and send an acknowledgement to the SC when the
      // message has successfully reached the MS irrespective of whether
      // there is memory available in the (U)SIM or ME. The message shall
      // not be automatically stored in the (U)SIM or ME.`
      // ~ 3GPP 23.038 clause 4

      handleReceivedAndAck(
        Cr.NS_OK, // ACK OK For Incomplete Class 0
        this._processReceivedSmsSegment(segment)
      );
    } else {
      lazy.gMobileMessageDatabaseService.saveSmsSegment(
        segment,
        (aRv, aCompleteMessage) => {
          handleReceivedAndAck(
            aRv, // Ack according to the result after saving
            aCompleteMessage
          );
        }
      );
    }
  },

  /**
   * nsIObserver interface.
   */
  observe(aSubject, aTopic, aData) {
    switch (aTopic) {
      case NS_PREFBRANCH_PREFCHANGE_TOPIC_ID:
        if (aData === RIL_DEBUG.PREF_RIL_DEBUG_ENABLED) {
          this._updateDebugFlag();
        } else if (aData === kPrefLastKnownSimMcc) {
          lazy.gSmsSegmentHelper.enabledGsmTableTuples =
            getEnabledGsmTableTuplesFromMcc();
        }
        break;
      case kDiskSpaceWatcherObserverTopic:
        if (DEBUG) {
          debug("Observe " + kDiskSpaceWatcherObserverTopic + ": " + aData);
        }
        this._reportSmsMemoryStatus(aData != "full");
        break;
      case NS_XPCOM_SHUTDOWN_OBSERVER_ID:
        // Release the CPU wake lock for handling the received SMS.
        this._releaseSmsHandledWakeLock();
        this.settingsObserver.removeSettingObserver(kSettingsDefaukltServiceId);
        Services.prefs.removeObserver(RIL_DEBUG.PREF_RIL_DEBUG_ENABLED, this);
        Services.obs.removeObserver(this, NS_XPCOM_SHUTDOWN_OBSERVER_ID);
        Services.obs.removeObserver(this, kDiskSpaceWatcherObserverTopic);
        break;
    }
  },

  _initImsSms() {
    if (DEBUG) {
      debug("_initImsSms");
    }
    this.imsSmsLinstensers = [];
    for (
      let serviceId = 0;
      serviceId < lazy.gRadioInterfaces.length;
      serviceId++
    ) {
      let smsProvider = new ImsSmsProvider(this, serviceId);
      this._imsSmsProviders.push(smsProvider);
    }
  },

  _isIms(aServiceId) {
    let imsHandler = lazy.gImsRegService.getHandlerByServiceId(aServiceId);
    if (!imsHandler) {
      if (DEBUG) {
        debug("_isIms: no imsHandler ");
      }
      return false;
    }
    //TODO: check capabilities CAPABILITY_TYPE_SMS
    let isImsSms =
      imsHandler.enabled &&
      imsHandler.capability > Ci.nsIImsRegHandler.IMS_CAPABILITY_UNKNOWN;

    if (DEBUG) {
      debug("_isIms: return Ims SMS service " + isImsSms);
    }
    return isImsSms;
  },

  handleSettingChanged(aName, aResult) {
    if (DEBUG) {
      debug(aName + " is set to " + aResult);
    }
    switch (aName) {
      case kSettingsDefaukltServiceId:
        if (aResult < 0 || aResult >= lazy.gRadioInterfaces.length) {
          this.smsDefaultServiceId = 0;
        } else {
          this.smsDefaultServiceId = aResult;
        }
        break;
    }
  },
};

/**
 * Get enabled GSM national language locking shift / single shift table pairs
 * for current SIM MCC.
 *
 * @return a list of pairs of national language identifiers for locking shift
 * table and single shfit table, respectively.
 */
function getEnabledGsmTableTuplesFromMcc() {
  let mcc = Services.prefs.getCharPref(kPrefLastKnownSimMcc, 0);
  let tuples = [
    [lazy.RIL.PDU_NL_IDENTIFIER_DEFAULT, lazy.RIL.PDU_NL_IDENTIFIER_DEFAULT],
  ];
  let extraTuples = lazy.RIL.PDU_MCC_NL_TABLE_TUPLES_MAPPING[mcc];
  if (extraTuples) {
    tuples = tuples.concat(extraTuples);
  }

  return tuples;
}

function SmsSendingScheduler(aServiceId) {
  this._serviceId = aServiceId;
  this._queue = [];
  this._oldCapbilityIsVowifi = null;

  Services.obs.addObserver(this, kSmsDeletedObserverTopic);
  Services.obs.addObserver(this, NS_XPCOM_SHUTDOWN_OBSERVER_ID);
}
SmsSendingScheduler.prototype = {
  QueryInterface: ChromeUtils.generateQI([
    Ci.nsIMobileConnectionListener,
    Ci.nsIImsRegListener,
  ]),

  _serviceId: 0,
  _queue: null,

  /**
   * Ensure the handler is listening on MobileConnection events.
   */
  _ensureMoboConnObserverRegistration() {
    let connection = lazy.gMobileConnectionService.getItemByServiceId(
      this._serviceId
    );
    let imsReg = lazy.gImsRegService.getHandlerByServiceId(this._serviceId);

    if (connection) {
      try {
        connection.registerListener(this);
      } catch (e) {
        if (DEBUG) {
          debug("Voice listener has been registered");
        }
      }
    }

    if (imsReg) {
      try {
        imsReg.registerListener(this);
      } catch (e) {
        if (DEBUG) {
          debug("Ims listener has been registered");
        }
      }
    }
  },

  /**
   * Ensure the handler is not listening on MobileConnection events.
   */
  _ensureMoboConnObserverUnregistration() {
    let connection = lazy.gMobileConnectionService.getItemByServiceId(
      this._serviceId
    );
    let imsReg = lazy.gImsRegService.getHandlerByServiceId(this._serviceId);

    if (connection) {
      try {
        connection.unregisterListener(this);
      } catch (e) {
        if (DEBUG) {
          debug("Voice listener has been unregistered");
        }
      }
    }

    if (imsReg) {
      try {
        imsReg.unregisterListener(this);
      } catch (e) {
        if (DEBUG) {
          debug("Ims listener has been unregistered");
        }
      }
    }
  },

  /**
   * Schedule a sending request.
   *
   * @param aSendingRequest
   *        An object with the following properties:
   *        - messageId
   *          The messageId in MobileMessageDB.
   *        - onSend
   *          The callback to invoke to trigger a sending or retry.
   *        - onCancel
   *          The callback to invoke when the request is canceled and won't
   *          retry.
   */
  schedule(aSendingRequest) {
    if (aSendingRequest) {
      if (DEBUG) {
        debug(
          "scheduling message: messageId=" +
            aSendingRequest.messageId +
            ", serviceId=" +
            this._serviceId
        );
      }
      this._ensureMoboConnObserverRegistration();
      this._queue.push(aSendingRequest);

      // Keep the queue in order to guarantee the sending order matches user
      // expectation.
      this._queue.sort(function (a, b) {
        return a.messageId - b.messageId;
      });
    }

    this.send();
  },

  isVoWifiConnected() {
    //FIXME
    let imsHandler = lazy.gImsRegService.getHandlerByServiceId(this._serviceId);
    if (!imsHandler) {
      if (DEBUG) {
        debug("isVoWifiConnected: no imsHandler ");
      }
      return false;
    }

    let capability = imsHandler.enabled && imsHandler.capability;
    return (
      capability === Ci.nsIImsRegHandler.IMS_CAPABILITY_VOICE_OVER_WIFI ||
      capability === Ci.nsIImsRegHandler.IMS_CAPABILITY_VIDEO_OVER_WIFI
    );
  },

  /**
   * Send all requests in the queue if voice connection is available.
   */
  send() {
    let connection = lazy.gMobileConnectionService.getItemByServiceId(
      this._serviceId
    );

    // If the voice connection is temporarily unavailable, pend the request.
    let voiceInfo = connection && connection.voice;
    let voiceConnected = voiceInfo && voiceInfo.connected;
    if (!voiceConnected && !this.isVoWifiConnected()) {
      if (DEBUG) {
        debug("Voice connection is temporarily unavailable. Skip sending.");
      }
      return;
    }

    let snapshot = this._queue;
    this._queue = [];
    let req;
    while ((req = snapshot.shift())) {
      req.onSend();
    }

    // The sending / retry could fail and being re-scheduled immediately.
    // Only unregister listener when the queue is empty after retries.
    if (this._queue.length === 0) {
      this._ensureMoboConnObserverUnregistration();
    }
  },

  /**
   * nsIObserver interface.
   */
  observe(aSubject, aTopic, aData) {
    switch (aTopic) {
      case kSmsDeletedObserverTopic:
        let deletedInfo = aSubject.QueryInterface(Ci.nsIDeletedMessageInfo);
        if (DEBUG) {
          debug(
            "Observe " +
              kSmsDeletedObserverTopic +
              ": " +
              JSON.stringify(deletedInfo)
          );
        }

        if (deletedInfo && deletedInfo.deletedMessageIds) {
          for (let i = 0; i < this._queue.length; i++) {
            let id = this._queue[i].messageId;
            if (deletedInfo.deletedMessageIds.includes(id)) {
              if (DEBUG) {
                debug("Deleting message with id=" + id);
              }
              this._queue
                .splice(i, 1)[0]
                .onCancel(Ci.nsIMobileMessageCallback.NOT_FOUND_ERROR);
            }
          }
        }
        break;
      case NS_XPCOM_SHUTDOWN_OBSERVER_ID:
        this._ensureMoboConnObserverUnregistration();
        Services.obs.removeObserver(this, NS_XPCOM_SHUTDOWN_OBSERVER_ID);
        Services.obs.removeObserver(this, kSmsDeletedObserverTopic);

        // Cancel all pending requests and clear the queue.
        for (let req of this._queue) {
          req.onCancel(Ci.nsIMobileMessageCallback.NO_SIGNAL_ERROR);
        }
        this._queue = [];
        break;
    }
  },

  /**
   * nsIMobileConnectionListener implementation.
   */
  notifyVoiceChanged() {
    let connection = lazy.gMobileConnectionService.getItemByServiceId(
      this._serviceId
    );
    let voiceInfo = connection && connection.voice;
    let voiceConnected = voiceInfo && voiceInfo.connected;
    if (voiceConnected) {
      if (DEBUG) {
        debug("Voice connected. Resend pending requests.");
      }

      this.send();
    }
  },

  // Unused nsIMobileConnectionListener methods.
  notifyDataChanged() {},
  notifyDataError(message) {},
  notifyCFStateChanged(action, reason, number, timeSeconds, serviceClass) {},
  notifyEmergencyCbModeChanged(active, timeoutMs) {},
  notifyOtaStatusChanged(status) {},
  notifyRadioStateChanged() {},
  notifyClirModeChanged(mode) {},
  notifyLastKnownNetworkChanged() {},
  notifyLastKnownHomeNetworkChanged() {},
  notifyNetworkSelectionModeChanged() {},
  notifyDeviceIdentitiesChanged() {},
  notifySignalStrengthChanged() {},
  notifyModemRestart(reason) {},
  notifyScanResultReceived(scanResults) {},

  /**
   * nsIImsRegListener implementation.
   */
  // Unused nsIImsRegListener methods.
  notifyEnabledStateChanged(aEnabled) {},
  notifyPreferredProfileChanged(aProfile) {},

  notifyCapabilityChanged(aCapability, aUnregisteredReason) {
    let newCapbilityIsVowifi =
      aCapability == Ci.nsIImsRegHandler.IMS_CAPABILITY_VOICE_OVER_WIFI ||
      aCapability == Ci.nsIImsRegHandler.IMS_CAPABILITY_VIDEO_OVER_WIFI;
    if (newCapbilityIsVowifi !== this._oldCapbilityIsVowifi) {
      this._oldCapbilityIsVowifi = newCapbilityIsVowifi;
      if (newCapbilityIsVowifi) {
        if (DEBUG) {
          debug("Vowifi connected. Resend pending requests.");
        }

        this.send();
      }
    }
  },

  // Unused nsIImsRegListener methods.
  notifyRttEnabledStateChanged(aEnabled) {},
};

function PendingOp(aOptions, aCallback) {
  this.smsMessage = aOptions;
  this.callback = aCallback;
}
PendingOp.prototype = {
  smsMessage: null,
  callback: null,
};

function ImsSmsProvider(aSmsService, aServiceId) {
  if (DEBUG) {
    debug("ImsSmsProvider constructor service id: " + aServiceId);
  }
  this._imsToken = 0;
  this._serviceId = aServiceId;
  this._smsService = aSmsService;
  this._pendingOp = [];
  this._lastIncomingMsgs = [];
  // For sim io context.
  this.clientId = "IMS_" + aServiceId; //to fit for radiointerface clientId
  this.simIOContext = new lazy.SIM.Context(this);

  this._imsHandler = lazy.gImsRegService.getHandlerByServiceId(aServiceId);
  if (this._imsHandler) {
    if (this._imsHandler.imsMMTelFeature) {
      if (DEBUG) {
        debug("ImsSmsProvider[" + this._serviceId + "]: Try to setSmsListener");
      }
      this._imsHandler.imsMMTelFeature.setSmsListener(this);
    }
  }
}

ImsSmsProvider.prototype = {
  QueryInterface: ChromeUtils.generateQI([Ci.nsIImsSmsListener]),

  _imsToken: 0,
  _serviceId: 0,
  _smsService: null,
  _imsHandler: null,
  _pendingOp: null,
  simIOContext: null,
  _lastIncomingMsgs: null,

  getNextToken() {
    return this._imsToken++;
  },

  getSmsFormat() {
    let smsFormat = this._imsHandler.imsMMTelFeature.getSmsFormat();
    if (smsFormat == "unknown") {
      if (DEBUG) {
        debug(
          "ImsSmsProvider[" +
            this._serviceId +
            "]: getSmsFormat return " +
            smsFormat
        );
      }
      smsFormat = "3gpp"; //default sms format
    }
    return smsFormat;
  },

  sendSms(aOptions, aCallback) {
    let newToken = this.getNextToken();
    if (DEBUG) {
      debug(
        "ImsSmsProvider[" +
          this._serviceId +
          "][" +
          newToken +
          "]sendSMS aOptions: " +
          JSON.stringify(aOptions)
      );
    }

    aOptions.rilMessageToken = newToken;
    aOptions.langIndex =
      aOptions.langIndex || lazy.RIL.PDU_NL_IDENTIFIER_DEFAULT;
    aOptions.langShiftIndex =
      aOptions.langShiftIndex || lazy.RIL.PDU_NL_IDENTIFIER_DEFAULT;

    if (!aOptions.segmentSeq) {
      // Fist segment to send
      aOptions.segmentSeq = 1;
      aOptions.body = aOptions.segments[0].body;
      aOptions.encodedBodyLength = aOptions.segments[0].encodedBodyLength;
    }
    this._pendingOp[newToken] = new PendingOp(aOptions, aCallback);

    let isRetry = aOptions.retryCount > 0;
    let gsmPduHelper = this.simIOContext.GsmPDUHelper;
    gsmPduHelper.initWith();
    gsmPduHelper.writeMessage(aOptions);

    let length = gsmPduHelper.pduWriteIndex / 2;
    let pdu = gsmPduHelper.readHexOctetArray(length);
    let smsFormat = this.getSmsFormat();

    if (DEBUG) {
      debug(
        "ImsSmsProvider[" +
          this._serviceId +
          "][" +
          newToken +
          "]: smsFormat " +
          smsFormat
      );
    }

    this._imsHandler.imsMMTelFeature.sendSms(
      newToken,
      0,
      smsFormat,
      aOptions.SMSC,
      isRetry,
      length,
      pdu
    );
  },

  acknowledgeSms(aStatus) {
    if (this._lastIncomingMsgs.length) {
      let msg = this._lastIncomingMsgs.shift();
      if (DEBUG) {
        debug(
          "ImsSmsProvider[" +
            this._serviceId +
            "][" +
            msg.token +
            "]: acknowledgeSms messageRef: " +
            msg.messageRef +
            ", aStatus: " +
            aStatus
        );
      }

      let deliveryStatus = Ci.nsIImsMMTelFeature.DELIVER_STATUS_OK;
      if (aStatus === lazy.RIL.PDU_FCS_OK) {
        deliveryStatus = Ci.nsIImsMMTelFeature.DELIVER_STATUS_OK;
      } else if (aStatus === lazy.RIL.PDU_FCS_MEMORY_CAPACITY_EXCEEDED) {
        deliveryStatus = Ci.nsIImsMMTelFeature.DELIVER_STATUS_ERROR_NO_MEMORY;
      } else {
        deliveryStatus = Ci.nsIImsMMTelFeature.DELIVER_STATUS_ERROR_GENERIC;
      }

      this._imsHandler.imsMMTelFeature.acknowledgeSms(
        msg.token,
        msg.messageRef,
        deliveryStatus
      );
    } else if (DEBUG) {
      debug("acknowledgeSms: No Message could be ack");
    }
  },

  _notifyNewSmsMessage(aMessage) {
    let header = aMessage.header;
    // Concatenation Info:
    // - segmentRef: a modulo 256 counter indicating the reference number for a
    //               particular concatenated short message. '0' is a valid number.
    // - The concatenation info will not be available in |header| if
    //   segmentSeq or segmentMaxSeq is 0.
    // See 3GPP TS 23.040, 9.2.3.24.1 Concatenated Short Messages.
    let segmentRef =
      header && header.segmentRef !== undefined ? header.segmentRef : 1;
    let segmentSeq = (header && header.segmentSeq) || 1;
    let segmentMaxSeq = (header && header.segmentMaxSeq) || 1;
    // Application Ports:
    // The port number ranges from 0 to 49151.
    // see 3GPP TS 23.040, 9.2.3.24.3/4 Application Port Addressing.
    let originatorPort =
      header && header.originatorPort !== undefined
        ? header.originatorPort
        : Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID;
    let destinationPort =
      header && header.destinationPort !== undefined
        ? header.destinationPort
        : Ci.nsIGonkSmsService.SMS_APPLICATION_PORT_INVALID;

    let gonkSms = {
      QueryInterface: ChromeUtils.generateQI([Ci.nsIGonkSmsMessage]),
      smsc: aMessage.SMSC || null,
      sentTimestamp: aMessage.sentTimestamp,
      sender: aMessage.sender,
      pid: aMessage.pid,
      encoding: aMessage.encoding,
      messageClass: lazy.RIL.GECKO_SMS_MESSAGE_CLASSES.indexOf(
        aMessage.messageClass
      ),
      language: aMessage.language || null,
      segmentRef,
      segmentSeq,
      segmentMaxSeq,
      originatorPort,
      destinationPort,
      imsMessage: true,
      // MWI info:
      mwiPresent: !!aMessage.mwi,
      mwiDiscard: aMessage.mwi ? aMessage.mwi.discard : false,
      mwiMsgCount: aMessage.mwi ? aMessage.mwi.msgCount : 0,
      mwiActive: aMessage.mwi ? aMessage.mwi.active : false,
      // CDMA related attributes:
      cdmaMessageType: aMessage.messageType || 0,
      cdmaTeleservice: aMessage.teleservice || 0,
      cdmaServiceCategory: aMessage.serviceCategory || 0,
      body: aMessage.body || null,
    };

    this._smsService.notifyMessageReceived(
      this._serviceId,
      gonkSms,
      aMessage.data || [],
      aMessage.data ? aMessage.data.length : 0
    );
  },

  /**
   * Helper for processing sent multipart SMS.
   */
  _processSentSmsSegment(aOptions, aCallback) {
    // Setup attributes for sending next segment
    let next = aOptions.segmentSeq;
    aOptions.body = aOptions.segments[next].body;
    aOptions.encodedBodyLength = aOptions.segments[next].encodedBodyLength;
    aOptions.segmentSeq = next + 1;
    this.sendSms(aOptions, aCallback);
  },

  //nsIImsSmsListener implementation
  onSendSmsResult(aToken, aMessageRef, aStatus, aReason, aNetworkErrorCode) {
    if (DEBUG) {
      debug(
        "ImsSmsProvider[" +
          this._serviceId +
          "][" +
          aToken +
          "]: onSendSmsResult status: " +
          aStatus
      );
    }
    let pendOp = this._pendingOp[aToken];
    if (pendOp) {
      pendOp.smsMessage.messageRef = aMessageRef;
      pendOp.smsMessage.errorCode = aNetworkErrorCode;
      if (aStatus == Ci.nsIImsMMTelFeature.SEND_STATUS_OK) {
        if (
          pendOp.smsMessage.segmentMaxSeq > 1 &&
          pendOp.smsMessage.segmentSeq < pendOp.smsMessage.segmentMaxSeq
        ) {
          // Not last segment
          this._processSentSmsSegment(pendOp.smsMessage, pendOp.callback);
          delete this._pendingOp[aToken];
          return;
        } else if (pendOp.smsMessage.requestStatusReport) {
          // Last segment sent with success.
          if (DEBUG) {
            debug(
              "waiting SMS-STATUS-REPORT for messageRef " +
                pendOp.smsMessage.messageRef
            );
          }
        }
      }

      let keepCallback = pendOp.callback({ status: aStatus, reason: aReason });
      if (!keepCallback) {
        delete this._pendingOp[aToken];
      }
    }
  },

  onSmsStatusReportReceived(aToken, aFormat, aLength, aPdu) {
    if (DEBUG) {
      debug(
        "ImsSmsProvider[" +
          this._serviceId +
          "][" +
          aToken +
          ": onSmsStatusReportReceived"
      );
    }
    let gsmPduHelper = this.simIOContext.GsmPDUHelper;
    gsmPduHelper.initWith(aPdu);
    let [message, result] = gsmPduHelper.processReceivedSms(aLength);
    if (DEBUG) {
      debug(
        "New IMS SMS status report: " +
          JSON.stringify(message) +
          ", result: " +
          result
      );
    }

    let status = message.status;

    // 3GPP TS 23.040 9.2.3.15 `The MS shall interpret any reserved values as
    // "Service Rejected"(01100011) but shall store them exactly as received.
    if (
      status >= 0x80 ||
      (status >= lazy.RIL.PDU_ST_0_RESERVED_BEGIN &&
        status < lazy.RIL.PDU_ST_0_SC_SPECIFIC_BEGIN) ||
      (status >= lazy.RIL.PDU_ST_1_RESERVED_BEGIN &&
        status < lazy.RIL.PDU_ST_1_SC_SPECIFIC_BEGIN) ||
      (status >= lazy.RIL.PDU_ST_2_RESERVED_BEGIN &&
        status < lazy.RIL.PDU_ST_2_SC_SPECIFIC_BEGIN) ||
      (status >= lazy.RIL.PDU_ST_3_RESERVED_BEGIN &&
        status < lazy.RIL.PDU_ST_3_SC_SPECIFIC_BEGIN)
    ) {
      status = lazy.RIL.PDU_ST_3_SERVICE_REJECTED;
    }

    // Pending. Waiting for next status report.
    if (status >>> 5 == 0x01) {
      if (DEBUG) {
        this.debug("SMS-STATUS-REPORT: delivery still pending");
      }
      return;
    }

    let deliveryStatus =
      status >>> 5 === 0x00
        ? lazy.RIL.GECKO_SMS_DELIVERY_STATUS_SUCCESS
        : lazy.RIL.GECKO_SMS_DELIVERY_STATUS_ERROR;

    let pendOp = this._pendingOp[aToken];
    if (pendOp) {
      pendOp.callback({
        status: Ci.nsIImsMMTelFeature.STATUS_REPORT_STATUS_OK,
        deliveryStatus,
      });
      delete this._pendingOp[aToken];
    }

    this._imsHandler.imsMMTelFeature.acknowledgeSmsReport(
      aToken,
      message.messageRef,
      Ci.nsIImsMMTelFeature.STATUS_REPORT_STATUS_OK
    );
  },

  onSmsReceived(aToken, aFormat, aLength, aPdu) {
    if (DEBUG) {
      debug(
        "ImsSmsProvider[" + this._serviceId + "][" + aToken + "]: onSmsReceived"
      );
    }
    let gsmPduHelper = this.simIOContext.GsmPDUHelper;
    gsmPduHelper.initWith(aPdu);
    let [message, result] = gsmPduHelper.processReceivedSms(aLength);
    if (DEBUG) {
      debug("New IMS SMS: " + JSON.stringify(message) + ", result: " + result);
    }
    message.token = aToken;
    this._lastIncomingMsgs.push(message);
    this._notifyNewSmsMessage(message);
  },
};

var EXPORTED_SYMBOLS = ["SmsService"];
