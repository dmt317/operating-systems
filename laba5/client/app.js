const API_BASE_URL = 'http://localhost:8080/api';

async function fetchCurrentTemperature() {
    try {
        const response = await fetch(`${API_BASE_URL}/current_temperature`);
        const data = await response.json();
        document.getElementById('current-temperature').textContent =
            `Current Temperature: ${data.temperature}°C (at ${data.timestamp})`;
    } catch (error) {
        console.error('Error fetching current temperature:', error);
        document.getElementById('current-temperature').textContent =
            'Failed to fetch current temperature.';
    }
}

async function fetchAndDrawChart(endpoint, label) {
    try {
        const response = await fetch(`${API_BASE_URL}/average_temperature/${endpoint}`);
        const data = await response.json();

        if (data.length === 0) {
            console.warn(`No data available for ${endpoint}`);
            document.getElementById('chart-canvas').style.display = 'none';
            document.getElementById('current-temperature').textContent =
                `No data available for ${label.toLowerCase()} chart.`;
            return;
        }

        const labels = data.map(item => item.timestamp);
        const temperatures = data.map(item => item.temperature);

        const ctx = document.getElementById('chart-canvas').getContext('2d');
        document.getElementById('chart-canvas').style.display = 'block';

        if (window.currentChart) {
            window.currentChart.destroy();
        }

        window.currentChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: `${label} Average Temperature`,
                    data: temperatures,
                    borderColor: 'rgba(75, 192, 192, 1)',
                    backgroundColor: 'rgba(75, 192, 192, 0.2)',
                }]
            },
            options: {
                responsive: true,
                plugins: {
                    legend: {
                        position: 'top',
                    }
                },
                scales: {
                    x: {
                        title: {
                            display: true,
                            text: 'Timestamp'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Temperature (°C)'
                        }
                    }
                }
            }
        });
    } catch (error) {
        console.error(`Error fetching ${endpoint} data:`, error);
        document.getElementById('current-temperature').textContent =
            `Failed to fetch ${label.toLowerCase()} data.`;
    }
}

function attachEventListeners() {
    document.getElementById('btn-current').addEventListener('click', fetchCurrentTemperature);
    document.getElementById('btn-hourly').addEventListener('click', () => fetchAndDrawChart('hour', 'Hourly'));
    document.getElementById('btn-daily').addEventListener('click', () => fetchAndDrawChart('day', 'Daily'));
}

function init() {
    attachEventListeners();
}

document.addEventListener('DOMContentLoaded', init);
