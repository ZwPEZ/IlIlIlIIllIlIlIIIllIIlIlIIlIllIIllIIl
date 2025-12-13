import asyncio
from playwright.async_api import async_playwright
import os

async def main():
    async with async_playwright() as p:
        browser = await p.chromium.launch()
        page = await browser.new_page()
        await page.goto(f'file://{os.getcwd()}/test.html')
        await asyncio.sleep(2)
        await page.locator('#policy-container').screenshot(path='policy_component_v2.png')
        await browser.close()

asyncio.run(main())