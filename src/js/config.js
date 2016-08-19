module.exports = [
  {
    "type": "heading",
    "defaultValue": "Watchface Configuration"
  },
  {
    "type": "text",
    "defaultValue": "You can customize your watchface here."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Clocks"
      },
      {
      "type": "select",
      "messageKey": "SelectClock",
      "defaultValue": 'd',
      "label": "Preferred Clock Type",
      "options": [
          {
            "label": "Digital",
            "value": 'd'
          },
          {
            "label": "Analog",
            "value": 'a'
          }
        ]
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "LeftStripeColor",
        "defaultValue": "0x555555",
        "allowGray": true,
        "label": "Left Background Stripe"
      },
      {
        "type": "color",
        "messageKey": "RightStripeColor",
        "defaultValue": "0x555555",
        "allowGray": true,
        "label": "Right Background Stripe"
      },
      {
        "type": "color",
        "messageKey": "WatchBandColor",
        "defaultValue": "0xFFFFFF",
        "allowGray": true,
        "label": "Watchband (Middle Stripe)"
      },
      {
        "type": "color",
        "messageKey": "WatchFaceColor",
        "defaultValue": "0xFFFFFF",
        "allowGray": true,
        "label": "Watchface (Circle)"
      },
      {
        "type": "color",
        "messageKey": "BatteryColor",
        "defaultValue": "0xAAAAAA",
        "allowGray": true,
        "label": "Battery Bar"
      },
      {
        "type": "color",
        "messageKey": "TextColor",
        "defaultValue": "0x000000",
        "label": "Clock Text"
      },
      {
        "type": "color",
        "messageKey": "HandsColor",
        "defaultValue": "0x000000",
        "label": "Minute/Hour Hands (Analog)"
      },
      {
        "type": "color",
        "messageKey": "SecondHandColor",
        "defaultValue": "0xAAAAAA",
        "allowGray": true,
        "label": "Second Hand (Analog)"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Details"
      },
      {
        "type": "toggle",
        "messageKey": "BatteryBarToggle",
        "defaultValue": true,
        "label": "Draw Circular Battery Meter"
      },
      {
        "type": "toggle",
        "messageKey": "StrapDetails",
        "defaultValue": true,
        "label": "Draw Watchstrap Details"
      },
      {
        "type": "toggle",
        "messageKey": "HourDots",
        "defaultValue": true,
        "label": "Draw Clock Hour Markings"
      },
      {
        "type": "toggle",
        "messageKey": "InvertOutline",
        "defaultValue": false,
        "label": "Invert Outline Color"
      },
    ]
  },
  {
    "type": "text",
    "defaultValue": "When you have finished your selection, hit SAVE!"
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];