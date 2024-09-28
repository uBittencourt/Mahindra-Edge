import dash
import pytz
import requests
from dash import dcc, html
from datetime import datetime
import plotly.graph_objs as go
from dash.dependencies import Input, Output, State

# Constants for IP and port
IP_ADDRESS = "54.235.126.90"
PORT_STH = 8666
DASH_HOST = "0.0.0.0"  # Allow access from any IP

# Function to get obstacle data from the API
def get_obstacle_data(path, lastN):
    url = f"http://{IP_ADDRESS}:{PORT_STH}/STH/v1/contextEntities/type/Obstacle/id/urn:ngsi-ld:Obstacle:001/attributes/{path}?lastN={lastN}"

    headers = {
        'fiware-service': 'smart',
        'fiware-servicepath': '/'
    }
    response = requests.get(url, headers=headers)
    if response.status_code == 200:
        data = response.json()
        try:
            values = data['contextResponses'][0]['contextElement']['attributes'][0]['values']
            return values
        except KeyError as e:
            print(f"Key error: {e}")
            return []
    else:
        print(f"Error accessing {url}: {response.status_code}")
        return []

# Function to convert UTC timestamps to São Paulo time
def convert_to_sao_paulo_time(timestamps):
    utc = pytz.utc
    sao_paulo = pytz.timezone('America/Sao_Paulo')
    converted_timestamps = []
    for timestamp in timestamps:
        try:
            timestamp = timestamp.replace('T', ' ').replace('Z', '')
            converted_time = utc.localize(datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S.%f')).astimezone(sao_paulo)
        except ValueError:
            converted_time = utc.localize(datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S')).astimezone(sao_paulo)
        converted_timestamps.append(converted_time)
    return converted_timestamps


# Set lastN value
lastN = 1  # Get 1 most recent point at each interval

app = dash.Dash(__name__)

app.layout = html.Div([
    html.H1('Is there an obstacle?'),
    dcc.Graph(id='obstacles-graph'),
    dcc.Store(id='obstacles-data-store', data={'timestamps': [], 'left_values': [], 'front_values': [], 'right_values': []}),
    dcc.Interval(
        id='interval-component',
        interval=10*1000,  # 10 seconds
        n_intervals=0
    )
])

@app.callback(
    Output('obstacles-data-store', 'data'),
    Input('interval-component', 'n_intervals'),
    State('obstacles-data-store', 'data')
)
def update_data_store(n, stored_data):
    # Get obstacle data
    data_left = get_obstacle_data('left-obstacle', lastN)
    data_front = get_obstacle_data('front-obstacle', lastN)
    data_right = get_obstacle_data('right-obstacle', lastN)

    if data_left and data_front and data_right:
        # Extract values and timestamps
        left_values = [float(entry['attrValue']) for entry in data_left]
        front_values = [float(entry['attrValue']) for entry in data_front]
        right_values = [float(entry['attrValue']) for entry in data_right]
        timestamps = [entry['recvTime'] for entry in data_left]

        # Convert timestamps to São Paulo time
        timestamps = convert_to_sao_paulo_time(timestamps)

        # Append new data to stored data
        stored_data['timestamps'].extend(timestamps)
        stored_data['left_values'].extend(left_values)
        stored_data['front_values'].extend(front_values)
        stored_data['right_values'].extend(right_values)

        return stored_data

    return stored_data

@app.callback(
    Output('obstacles-graph', 'figure'),
    Input('obstacles-data-store', 'data')
)
def update_graph(stored_data):
    if stored_data['timestamps'] and stored_data['left_values'] and stored_data['front_values'] and stored_data['right_values']:
        # Create traces for the plot
        trace_obstacle_left = go.Scatter(
            x=stored_data['timestamps'],
            y=stored_data['left_values'],
            mode='lines+markers',
            name='Left Side',
            line=dict(color='orange')
        )
        trace_obstacle_front = go.Scatter(
            x=stored_data['timestamps'],
            y=stored_data['front_values'],
            mode='lines+markers',
            name='Front Side',
            line=dict(color='green')
        )
        trace_obstacle_right = go.Scatter(
            x=stored_data['timestamps'],
            y=stored_data['right_values'],
            mode='lines+markers',
            name='Right Side',
            line=dict(color='red')
        )

        # Create figure
        fig_obstacles = go.Figure(data=[trace_obstacle_left, trace_obstacle_front, trace_obstacle_right])

        # Update layout
        fig_obstacles.update_layout(
            title='Is there an obstacle?',
            xaxis_title='Timestamp',
            yaxis_title='Obstacle',
            hovermode='closest'
        )

        return fig_obstacles

    return {}

if __name__ == '__main__':
    app.run_server(debug=True, host=DASH_HOST, port=8050)
