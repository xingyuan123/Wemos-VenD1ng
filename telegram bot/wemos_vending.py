# written by chuan an
# MQTT integration by jun heng and mayank

from telegram.ext import Updater, CommandHandler, MessageHandler, Filters, CallbackQueryHandler, ConversationHandler
from telegram import Update
from telegram.ext import CallbackContext
from telegram import InlineKeyboardMarkup, InlineKeyboardButton, ParseMode
import telegram
import logging

from pymongo import MongoClient
import paho.mqtt.client as mqtt
MQTT_SERVER = "34.143.248.115" #fill ur own
MQTT_PATH = "Telebot" # fill ur own

from datetime import datetime 
import datetime
import time
import random
import matplotlib.pyplot as plt

logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                     level=logging.INFO)
logger = logging.getLogger(__name__)
TOKEN = '' # removed for security purposes

client = MongoClient('') # removed for security purposes
db = client.get_database('wemosvending_db')
users_info = db.users_info

cart_emoji = "\ud83d\uded2"
stop_sign = '\ud83d\udeab'
tick = '\u2714\ufe0f'
cross = '\u2716\ufe0f'

TOPUP_ACCOUNT, PROCEED_PAYMENT, DISPENSE_SNACK = range(3)

def setup():
    mqtt_client = mqtt.Client()
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    # client.tls_insecure_set(True)
    mqtt_client.connect(MQTT_SERVER, 1883, 60)
    mqtt_client.loop_start()
    return mqtt_client

def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print("Successfully connected to broker.")
    else:
        print("Connection failed with code: %d." %rc)
    mqtt_client.subscribe(MQTT_PATH + "/dispense")
    mqtt_client.subscribe("Telebot/machinestolen")

def on_message(mqtt_client, userdata, msg):
    print(msg.topic)
    if msg.topic == "Telebot/machinestolen":
        message = msg.payload.decode("utf-8")
        bot = telegram.Bot(token=TOKEN)
        bot.send_message(chat_id=772520752, text="Vending machine has been stolen!")

mqtt_client = setup()


def error_callback(update, context):
    """Log Errors caused by Updates."""
    logger.warning('Update "%s" caused error "%s"', update, context.error)

def start(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    username = update.message.from_user.username
    name = update.message.from_user.first_name
    account_info = users_info.find_one({'user_id':user_id})
    if account_info == None:
        localtime = time.localtime(time.time())
        date_now = str(localtime.tm_mday)+'/'+str(localtime.tm_mon)+'/'+str(localtime.tm_year)
        time_now = str(localtime.tm_hour)+':'+str(localtime.tm_min)+':'+str(localtime.tm_sec)
        time_stamp = date_now + ' ' + time_now
        new_user = {
            'user_id':user_id,
            'name':name,
            'username':username,
            'date_started': time_stamp,
            'balance':0

        }
        users_info.insert_one(new_user)
    context.bot.send_message(chat_id=user_id, text=cart_emoji + ' Hi '+name+ '! \n\n'
                                                                '<b>Welcome to WeMos Vending!</b>\n'
                                                                'Type /help to find out more.'
    , parse_mode=ParseMode.HTML)

def help(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    context.bot.send_message(user_id,   '<b><u>How to use:</u></b>\n\n'
                                        '1) /topup to top up money into your account.\n'
                                        '2) /checkbalance to check your account balance.\n'
                                        '3) /purchase to buy snacks.\n'
                                        '4) /view to see your past shopping records.\n'
                                        '5) /viewall to see all past shopping records (owners).\n'
                                        '6) /checkstock to check snacks quantity left (owners).\n'
                                        '7) /report to provide malfunctions feedback.\n\n',parse_mode = ParseMode.HTML)
def view(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    orders = db.users_orders
    my_orders = orders.find_one({'user_id':user_id})
    if my_orders == None:
        context.bot.send_message(chat_id=user_id, text="You have not made any purchases!", parse_mode = ParseMode.HTML)

    else:
        my_orders = orders.find({'user_id':user_id})
        result = ''
        for order in my_orders:
            sub_result = ''
            snack_selected = order['snack_selected']
            cost = order['cost']
            date_ordered = order['date_ordered']
            sub_result = f"<b>Time</b> : {date_ordered} | <b>Order</b>: {snack_selected} | <b>Cost</b> : {cost}\n"
            result += sub_result
        context.bot.send_message(chat_id=user_id, text=result, parse_mode = ParseMode.HTML)

def viewall(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    owners_info = db.owners_info
    owner = owners_info.find_one({'user_id':user_id})
    if owner == None:
        context.bot.send_message(chat_id=user_id, text="You do not have permission for this!", parse_mode = ParseMode.HTML)
    else:
        orders = db.users_orders
        all_orders = orders.find({})
        result = ''
        for order in all_orders:
            sub_result = ''
            snack_selected = order['snack_selected']
            cost = order['cost']
            date_ordered = order['date_ordered']
            name  = order['name']
            sub_result = f"<b>Time</b> : {date_ordered} | <b>User</b>: {name} | <b>Order</b>: {snack_selected} | <b>Cost</b> : {cost}\n"
            result += sub_result
        context.bot.send_message(chat_id=user_id, text=result, parse_mode = ParseMode.HTML)

def checkstock(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    owners_info = db.owners_info
    owner = owners_info.find_one({'user_id':user_id})
    if owner == None:
        context.bot.send_message(chat_id=user_id, text="You do not have permission for this!", parse_mode = ParseMode.HTML)
    else:
        snacks_info = db.snacks_info
        all_snacks = snacks_info.find({})
        snack_names = []
        quantity_left = []
        for snack in all_snacks:
            if snack['name'] == "Jackpot":
                continue
            snack_names.append(snack['name'])
            quantity_left.append(snack['quantity_left'])

        fig = plt.figure()
        plt.title("Stocks left", fontsize=20)
        plt.xlabel('Snacks', fontsize=18)
        plt.ylabel('Quantity', fontsize=18)

        plt.bar(snack_names, quantity_left, width=0.3,align='center')
        fig.savefig('stocks_left.jpg',bbox_inches='tight')
        context.bot.send_photo(chat_id=user_id, photo=open('stocks_left.jpg', 'rb'))
        # context.bot.send_message(chat_id=user_id, text=result, parse_mode = ParseMode.HTML)

def report(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    owners_info = db.owners_info
    owner = owners_info.find_one({})
    owner_handle = owner["username"]
    keyboard=[[InlineKeyboardButton("Contact", url=f"https://t.me/{owner_handle}")]]
    reply_markup = InlineKeyboardMarkup(keyboard)
    context.bot.send_message(user_id,'Contact the owner to feedback.', reply_markup=reply_markup)
            
def checkbalance(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    username = update.message.from_user.username
    name = update.message.from_user.first_name
    account_info = users_info.find_one({'user_id':user_id})
    balance = users_info.find_one({'user_id': user_id})['balance']
    context.bot.send_message(chat_id=user_id, text='Your account balance is: $<b><u>'+ str(round(balance,2)) + '</u></b>', parse_mode = ParseMode.HTML)
    

def topup(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    context.bot.send_message(user_id,'Please enter your top up amount:')
    return TOPUP_ACCOUNT

def topup_account(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    username = update.message.from_user.username
    name = update.message.from_user.first_name
    topup_amount = update.message.text
    users_info = db.users_info
    try:
        if float(topup_amount) >= 0.1:

            users_info.find_one_and_update({'user_id': user_id},
                                            {'$inc': {'balance': float(topup_amount)}})
            balance = users_info.find_one({'user_id': user_id})['balance']

            context.bot.send_message(chat_id=user_id, text=tick+' <b>$'+topup_amount+ '</b> has been successfully added.\n You have $<b><u>'+str(round(balance,2)) +'</u></b> in your account now.', parse_mode = ParseMode.HTML)
        else:
            context.bot.send_message(chat_id=user_id, text='Please enter a positive value of at least 0.1.\n<i>Type /topup to try again</i>', parse_mode = ParseMode.HTML)
    except:
        context.bot.send_message(chat_id=user_id, text=cross+' Invalid amount.\n<i>Type /topup to try again</i>', parse_mode = ParseMode.HTML)
    return ConversationHandler.END

def purchase(update: Update, context: CallbackContext):
    mqtt_client.publish(MQTT_PATH + "/dispense", 'hihi')
    user_id = update.message.from_user.id
    username = update.message.from_user.username
    name = update.message.from_user.first_name
    keyboard = []
    inner_keyboard = []
    snacks_info = db.snacks_info
    cursor = snacks_info.find({})
    for document in cursor:
        snack_name = document['name']
        inner_keyboard.append(InlineKeyboardButton(snack_name, callback_data=snack_name))
        if len(inner_keyboard) == 2 or snack_name == 'Jackpot':
            keyboard.append(inner_keyboard)
            inner_keyboard = []
    keyboard.append([InlineKeyboardButton("Cancel", callback_data="Cancel")])
    reply_markup = InlineKeyboardMarkup(keyboard)
    context.bot.send_message(chat_id=user_id, text='Hi '+ name+', what would you like to buy?', reply_markup=reply_markup)
    return PROCEED_PAYMENT

def proceed_payment(update: Update, context: CallbackContext):
    query = update.callback_query
    user_id = query.from_user.id
    username = query.from_user.username
    name = query.from_user.first_name
    if query.data == "Cancel":
        context.bot.send_message(user_id,'Cancelled!')
        return ConversationHandler.END
    
    snacks_info = db.snacks_info
    cursor = snacks_info.find({})
    for document in cursor:
        snack_name = document['name']
        if snack_name == query.data:
            cost = document['cost']
    query.edit_message_text(text="Selected option: {}".format(query.data)+ '\nThis costs $'+ str(cost))

    keyboard = [[InlineKeyboardButton("Yes", callback_data="Yes"), InlineKeyboardButton("Cancel", callback_data="Cancel")]]
    reply_markup = InlineKeyboardMarkup(keyboard)
    context.bot.send_message(chat_id=user_id, text='Would you like to proceed?', reply_markup=reply_markup)
    context.user_data["snack_selected"] = query.data
    context.user_data["cost"] = cost
    return DISPENSE_SNACK

def dispense_snack(update: Update, context: CallbackContext):
    query = update.callback_query
    user_id = query.from_user.id
    username = query.from_user.username
    name = query.from_user.first_name
    if query.data == "Cancel":
        context.bot.send_message(user_id,'Cancelled!')
        return ConversationHandler.END
    
   
    query.edit_message_text(text="Selected option: {}".format(query.data))
    users_info = db.users_info
    users_orders = db.users_orders
    snacks_info = db.snacks_info
    account_info = users_info.find_one({'user_id':user_id})
    balance = users_info.find_one({'user_id': user_id})['balance']
    cost = context.user_data["cost"]
    snack_selected = context.user_data["snack_selected"]

    if balance < cost:
        context.bot.send_message(chat_id=user_id, text='Insufficient funds!')
        return ConversationHandler.END

    balance -= cost
    balance = round(balance,2)
    users_info.find_one_and_update({'user_id': user_id},
                                    {'$set': {'balance': balance}})
    
    localtime = time.localtime(time.time())
    date_now = str(localtime.tm_mday)+'/'+str(localtime.tm_mon)+'/'+str(localtime.tm_year)
    time_now = str(localtime.tm_hour)+':'+str(localtime.tm_min)+':'+str(localtime.tm_sec)
    time_stamp = date_now + ' ' + time_now
    new_order = {
            'user_id':user_id,
            'name':name,
            'username':username,
            'date_ordered': time_stamp,
            'snack_selected': snack_selected,
            'cost': cost
        }
    users_orders.insert_one(new_order)

    # tell server to dispense. MQTT code
    
    if snack_selected == "Jackpot":
        jackpot_number = random.randint(1,10)
        if jackpot_number > 0: #set to 100% for demo
            context.bot.send_message(chat_id=user_id, text='You won the Jackpot! :)')
        else:
            context.bot.send_message(chat_id=user_id, text='You did not win the Jackpot.. :(')
            return ConversationHandler.END
    mqtt_client.publish(MQTT_PATH + "/dispense", snack_selected)
    
    snacks_info.find_one_and_update({'name': snack_selected},
                                     {'$inc': {'quantity_left': -1}})

    
    context.bot.send_message(chat_id=user_id, text='Transaction successful.\n You have $<b><u>'+str(balance) +'</u></b> in your account now.', parse_mode = ParseMode.HTML)
    context.bot.answer_callback_query(callback_query_id=query.id, text="Thank you please come again :)", show_alert=True)

    return ConversationHandler.END

def timeout(update, context):
    try:
        user_id = update.message.from_user.id
    except:
        query = update.callback_query
        user_id = query.from_user.id

    context.bot.send_message(chat_id=user_id, text='Timeout. \n<i>Enter the command to start over again.</i>', parse_mode=ParseMode.HTML)
    return ConversationHandler.END

def main():
    updater = Updater(TOKEN, use_context = True)
    # Get the dispatcher to register handlers
    dp = updater.dispatcher
    # Create command handlers
    dp.add_handler(CommandHandler("start", start))
    dp.add_handler(CommandHandler("help", help))
    dp.add_handler(CommandHandler("checkbalance", checkbalance))
    dp.add_handler(CommandHandler("view", view)) 
    dp.add_handler(CommandHandler("viewall", viewall))
    dp.add_handler(CommandHandler("report", report))
    dp.add_handler(CommandHandler("checkstock", checkstock))

    dp.add_error_handler(error_callback)
    conv_handler = ConversationHandler(
        entry_points=[CommandHandler('topup', topup), CommandHandler('purchase', purchase)],

        states={
            TOPUP_ACCOUNT : [MessageHandler(Filters.text, topup_account)],
            PROCEED_PAYMENT : [CallbackQueryHandler(proceed_payment)],
            DISPENSE_SNACK : [CallbackQueryHandler(dispense_snack)],
            ConversationHandler.TIMEOUT : [MessageHandler(Filters.command | Filters.text, timeout),CallbackQueryHandler(timeout)]
        },

        fallbacks=[CommandHandler('topup', topup), CommandHandler('purchase', purchase)],
        per_chat = False,
        conversation_timeout=45
    )
    dp.add_handler(conv_handler)

    updater.start_polling()
    
    # Run the bot until you press Ctrl-C or the process receives SIGINT,
    # SIGTERM or SIGABRT. This should be used most of the time, since
    # start_polling() is non-blocking and will stop the bot gracefully.
    updater.idle()


if __name__ == '__main__':
    main()

# https://api.telegram.org/bot<BOTID>/getUpdates